flags = --std=c++11 -W -Wall -Wextra -pedantic -Werror # -O3

all: test

main: stack_allocator.h main.cpp
	g++ $(flags) main.cpp -o main

test: xorlist.h test.cpp
	g++ $(flags) test.cpp -lgtest -o test
