import argparse
import yaml
from runner import Runner


def parse_args():
    parser = argparse.ArgumentParser(description='some description', formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument('--config_path', type=str, required=True, help='Path to configuration file in YAML format')
    args = parser.parse_args()
    return args

def load_config(path):
    with open(path, 'r') as file:
        doc = yaml.load(file, yaml.FullLoader)
    return doc


if __name__ == '__main__':
    print('starting')
    args = parse_args()
    config = load_config(args.config_path)
    runner = Runner(
        config['stream_source']['socket']['ip'],
        config['stream_source']['socket']['port'],
        config['reciver']['broadcast'],
        config['reciver']['buffer_size']
    )
    runner.run()
