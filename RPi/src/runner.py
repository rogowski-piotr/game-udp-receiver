from rx import create
from reciver import DatagramReciver
from packets import unpack_udp_packet


class Runner():
    def __init__(self, source_ip, source_port, broadcast, buffer_size):
        self.source_ip = source_ip
        self.source_port = source_port
        self.broadcast = broadcast
        self.buffer_size = buffer_size
        None

    def run(self):        
        reciver = DatagramReciver(
            self.source_ip,
            self.source_port,
            self.broadcast,
            self.buffer_size
        )
        source = create(reciver.run)
        source.subscribe(
            on_next = lambda i: print(f'Recived: {len(i)}'),
            on_error = lambda e: print("Error Occurred: {0}".format(e)),
            on_completed = lambda: print("Done!"),
        )