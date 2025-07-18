import argparse
import json
import sys


def main():
    parser = argparse.ArgumentParser(description='Check for banned words')
    parser.add_argument('--solution', type=str, required=True, dest='solution_path')
    parser.add_argument('--banned-words', type=str, required=True, dest='banned_words_json_path')
    args = parser.parse_args()

    with open(args.banned_words_json_path, 'r') as banned_words_json_file:
        banned_words_json = json.load(banned_words_json_file)
        banned_words = banned_words_json.get('banned_words', [])

    with open(args.solution_path, 'r') as solution_file:
        file_content = solution_file.read()

    for word in banned_words:
        if word in file_content:
            sys.exit(f'Word {word} is banned!')


if __name__ == '__main__':
    main()