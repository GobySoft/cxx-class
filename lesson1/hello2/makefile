all: hello

hello.o:
	g++ -c hello.cpp

main.o:
	g++ -c main.cpp

hello: main.o hello.o
	g++ main.o hello.o -o hello
