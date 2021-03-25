all: pong

pong: pong.o
	gcc -o pong pingpong-preempcao.o ppos.o queue.o

pong.o: ppos.o
	 gcc -c -Wall pingpong-preempcao.c

ppos.o: ppos.c
	gcc -c -Wall ppos.c

clean:
	 rm pingpong-preempcao.o ppos.o pong