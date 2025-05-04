BER: buildBER runBER graph

buildBER:
	g++ TestBER.cpp -o TestBER.exe
runBER: buildBER
	./TestBER.exe
graph:
	python utils/graph.py

decoder: buildDEC runDEC

buildDEC:
	g++ TestDecoder.cpp -o TestDecoder.exe
runDEC: buildDEC
	./TestDecoder.exe
