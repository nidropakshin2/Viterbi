all: build run
build:
	g++ Test.cpp -o test.exe
run: build
	./test.exe