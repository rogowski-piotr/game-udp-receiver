import socket
import sys


class DatagramListener():
    def __init__(self, ip_address, port, broadcast=True, buffer_size=4096):
        self.ip_address = '' if broadcast else ip_address
        self.port = port
        self.buffer_size = buffer_size

    def run(self):
        socket_udp = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        server_address = (self.ip_address, self.port)
        socket_udp.bind(server_address)
        print("Listening press Ctrl+c to exit the program")

        while True:
            data, address = socket_udp.recvfrom(self.buffer_size)
            print("Server received: ", data.decode('utf-8'), "\n\n")