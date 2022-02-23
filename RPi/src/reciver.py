import socket
import sys


class DatagramReciver():
    def __init__(self, ip_address, port, broadcast=True, buffer_size=4096):
        self.ip_address = '' if broadcast else ip_address
        self.port = port
        self.buffer_size = buffer_size
        self.socket_udp = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.socket_udp.bind((self.ip_address, self.port))

    def run(self, observer, scheduler):
        while True:
            data, address = self.socket_udp.recvfrom(self.buffer_size)
            observer.on_next(data)