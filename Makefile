COMPILER = gcc
all: part1 part2

part1:
	$(COMPILER) -o p1  part1.c -lpthread

part2:
	$(COMPILER) -o p2p testthread.c mypthread.c queue.c
	
clean:
	rm -rf *.o