import argparse
import json
import sys


def main():
  parser = argparse.ArgumentParser(description='Check for bannded words')
  parser.add_argument('--solution', type=str, required=True, dest='solution_path')
  parser.add_argument('--bannded-words', type=str, required=True, dest='bannded_words_json_path')
  parser.parse_args()

  with open(parser.bannded_words_json_path, 'r') as banned_words_json_file:
    banned_words_json = json.load(banned_words_json_file)
    banned_words = banned_words_json.get('banned_words', [])

  with open(solution_path, 'r') as solution_file:
    file_content = solution_file.read()

  for word in banned_words:
    if word in file_content:
      sys.exit(f'Word {word} is banned!')


if __name__ == 'main':
  main()