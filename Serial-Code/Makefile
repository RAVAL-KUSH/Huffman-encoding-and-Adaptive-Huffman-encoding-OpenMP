cc = g++
flags = -pg
profFlags = 

all: launch profiler

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
