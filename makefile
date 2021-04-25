oFile = unionJoin.out

$(oFile): $(wildcard *.cpp)
	g++ *.cpp -O3 -o $(oFile)

run: $(oFile)
	./$(oFile)
