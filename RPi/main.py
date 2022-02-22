import argparse
import yaml
from server import DatagramListener


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
    listener = DatagramListener(
        config['stream_source']['socket']['ip'],
        config['stream_source']['socket']['port'],
        config['reciver']['broadcast'],
        config['reciver']['buffer_size']
    )
    listener.run()
