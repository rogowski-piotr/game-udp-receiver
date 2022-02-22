import argparse
import yaml
from rx import create
from reciver import DatagramReciver


def parse_args():
    parser = argparse.ArgumentParser(description='some description', formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument('--config_path', type=str, required=True, help='Path to configuration file in YAML format')
    args = parser.parse_args()
    return args

def load_config():
    with open('config.yml', 'r') as file:
        doc = yaml.load(file, yaml.FullLoader)
    return doc


if __name__ == '__main__':
    print('starting')
    config = load_config()
    args = parse_args()
    reciver = DatagramReciver(
        config['stream_source']['socket']['ip'],
        config['stream_source']['socket']['port'],
        config['reciver']['broadcast'],
        config['reciver']['buffer_size']
    )
    source = create(reciver.run)
    source.subscribe(
        on_next = lambda i: print("Received {0}".format(i)),
        on_error = lambda e: print("Error Occurred: {0}".format(e)),
        on_completed = lambda: print("Done!"),
    )
