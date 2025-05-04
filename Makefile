all: build run graph

build:
	g++ Test.cpp -o test.exe
run: build
	./test.exe
graph:
	python graph.py