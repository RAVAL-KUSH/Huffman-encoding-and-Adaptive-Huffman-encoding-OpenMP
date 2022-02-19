cc = g++
flags = -pg
profFlags = -s

all: launch profiler

huffman:
	$(cc) huffman.cpp -o main $(flags)

ADhuffman:
	$(cc) adaptiveHuffman.cpp -o main $(flags)

launch:
	./main

profiler:
	gprof $(profFlags) main gmon.out >gprof-out.txt
