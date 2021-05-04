oFile = unionJoin.out

$(oFile): $(wildcard *.cpp) $(wildcard *.h)
	g++ *.cpp -O3 -o $(oFile)

run: $(oFile)
	./$(oFile)
