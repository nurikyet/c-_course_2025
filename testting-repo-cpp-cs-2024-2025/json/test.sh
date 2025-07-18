#! /usr/bin/env bash

echo "begin testing $1"


echo "Начинаем билдить"
echo "Build with g++"
mkdir build
cp -r testdata build
cd build
cmake -DCMAKE_CXX_COMPILER=g++-13 ..
if [[ ! $? -eq 0 ]]
then
  echo "Сборка cmake с g++ не сработала"
  exit 1
fi
make
if [[ ! $? -eq 0 ]]
then
  echo "Make с g++ не сработал"
  exit 1
fi
echo "Build with g++ achieved"


echo "Running tests"
./$1_tests
if [[ ! $? -eq 0 ]]
then
  echo "Тесты не пройдены"
  exit 1
fi

echo "Running stress tests"
./$1_stress_tests
if [[ ! $? -eq 0 ]]
then
  echo "Тесты не пройдены"
  exit 1
fi
echo "Google tests achieved"

echo "build extra stress tests"
g++-13 -std=c++20 ../extra_stress_test.cpp -o $1_extra_stress_test
./$1_extra_stress_test
if [[ ! $? -eq 0 ]]
then
  echo "Тесты не пройдены"
  exit 1
fi
echo "Google tests achieved"

echo "Попробуем valgrind!"
valgrind --leak-check=yes --log-file=log.txt ./$1_tests
echo "Valgrind log:"
cat log.txt
python3 ../valgrind_parser.py
if [[ ! $? -eq 0 ]]
then
  echo "А кто украл free?"
  exit 1
fi
echo "Valgrind achieved"


echo "Вы потрясающие!"
