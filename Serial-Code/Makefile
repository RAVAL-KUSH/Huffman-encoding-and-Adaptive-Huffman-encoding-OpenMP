cc = g++
flags = -fopenmp
profFlags = 

all: launch

huffman:
	$(cc) huffman.cpp -o main $(flags)
	./main

ADhuffman:
	$(cc) adaptiveHuffman.cpp -o main $(flags)
	./main

launch:
	./main

profiler:
	gprof $(profFlags) main gmon.out >gprof-out.txt
