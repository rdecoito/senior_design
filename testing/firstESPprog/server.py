from socket import socket, AF_INET, SOCK_DGRAM

serverSocket = socket(AF_INET, SOCK_DGRAM)
serverSocket.bind(('', 8765))
#serverSocket.settimeout(0)

while True:
    try:
        message, address = serverSocket.recvfrom(1024)
    except BlockingIOError:
        None
    else:
        print(f'Received message from {address[0]}:{address[1]}\n{message}')
        serverSocket.sendto('Response obtained'.encode(), address)
