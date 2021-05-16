oFile = unionJoin.out

$(oFile): $(wildcard *.cpp) $(wildcard *.h) $(wildcard **/*.cpp) $(wildcard **/*.h)
	g++ **/*.cpp *.cpp -O3 -o $(oFile)

run: $(oFile)
	./$(oFile)

debug: $(wildcard *.cpp) $(wildcard *.h) $(wildcard **/*.cpp) $(wildcard **/*.h)
	g++ **/*.cpp *.cpp -g -o $(oFile)
	gdb -ex run ./$(oFile)
