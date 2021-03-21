'use-strict'
/*
    Author: Landon DeCoito
    Group: ECE 493 Senior Design Team 13, Spring 2021, George Mason University
    Last Edit Date: March 21, 2021
    
    Description:
        A NodeJS Lambda event handler for API Gateway REST API POST requests
        associated with the IoT Module (IM) model and version listed below.
        
        IM model number: 1
        IM model description: User forehead temperature checker
        IM version: 0.1
*/
const aws = require('aws-sdk');
const dynamodb = new aws.DynamoDB({apiVersion: '2012-08-10'});

// A mapping between parameter names in a POST body and the RegExp
// that should validate said parameter.
const regExpValidations = {
    IMID: /^[0-9a-f]{3}$/i, // 12-bit number, 3-digit hex
    TPMID: /^[0-9a-f]{3}$/i, // 12-bit number, 3-digit hex
    CPMID: /^[0-9a-f]{3}$/i, // 12-bit number, 3-digit hex
    timestamp: /^[0-9a-f]{8}$/i, // 32-bit number, 8-digit hex
    temp: /^[0-9a-f]{4}$/i, // 16-bit number, 4-digit hex
};

exports.handler = async (event, context) => {
    /*
    Standard Body:
    {
        "IMID": string (3-digit hex number)
        "TPMID": string (3-digit hex number),
        "CPMID": string (3-digit hex number),
        "has_mm": boolean,
        "mm_error": boolean,
        "trans_error": boolean,
        "im_error": boolean,
        "timestamp": string (8-digit hex number (unix timestamp)),
        "temp": number (4-digit hex number (10 * degrees C))
    }
    
    door_interaction object
    {
        "TPMID": number,
        "CPMID": number,
        "has_mm": boolean,
        "mm_reports_err": boolean,
        "mm_im_transmission_err": boolean,
        "im_reports_err": boolean,
        "timestamp": number,
        "temperature": number
    }
    */
    
    let postObj;
    let body;
    
    // Take the body of the POST request and transform it to the format we want
    // for the database. Gonna be doing error checking all the way through;
    // we want this handler to be as airtight as possible.
    try {
        body = JSON.parse(event.body);
        console.log('Received POST request body:', body);
        
        // Check that all the string parameters are formatted properly
        for (const param in regExpValidations) {
            if (!regExpValidations[param].test(body[param])) {
                return {
                    statusCode: 400,
                    body: JSON.stringify('Error: string params formatted incorrectly')
                };
            }
        }
        
        // Calculate the temperature. The only assumption I'm making about the
        // AWS Lambda architecture is that ints are at least 16 bits.
        let temp = parseInt(body.temp, 16);
        if (temp & 0x8000) { // if (temp is a negative number)
            // Sign extend. Use -1 so we don't have to assume the size of an int
            temp |= (-1 & ~0xFFFF); 
        }
        temp /= 10; // transmitted temperature  = 10 * degreesC, so divide by 10
        
        postObj = {
            TPMID: { N: parseInt(body.TPMID, 16).toString() },
            CPMID: { N: parseInt(body.CPMID, 16).toString() },
            IMID: { N: parseInt(body.IMID, 16).toString() },
            temperature: { N: temp.toString() },
            has_mm: { BOOL: body.has_mm },
            mm_reports_err: { BOOL: body.mm_error },
            mm_im_transmission_err: { BOOL: body.trans_error },
            im_reports_err: { BOOL: body.im_error },
            timestamp: { N: parseInt(body.timestamp, 16).toString() }
        };
        
        for (const param in postObj) {
            for (const val in postObj[param]) {
                if (postObj[param][val] == null) { // abstract equality (==) is INTENDED
                    console.error(`Encountered postObj[${param}][${val}]: ${postObj[param][val]}`);
                    return {
                        statusCode: 400,
                        body: JSON.stringify('Error: one or more params null or wrong type')
                    };
                }
            }
        }
    } catch (e) {
        console.error("Error trying to transform POST body", e);
        return {
            statusCode: 400,
            body: JSON.stringify('Error: ')
        }
    }
    
    const putParams = {
        TableName: 'door_interaction',
        Item: postObj,
        ReturnConsumedCapacity: 'NONE'
    };
    
    return dynamodb.putItem(putParams).promise().then(() => {
        return {
            statusCode: 200,
            body: JSON.stringify("Success: Data stored")
        };
    }).catch((err) => {
        console.error("Error trying to put item into DynamoDB table");
        console.error(err);
        return {
            statusCode: 500,
            body: JSON.stringify("Error: Failed to put item into database")
        };
    });
};
