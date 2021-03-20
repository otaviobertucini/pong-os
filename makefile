all: pong

pong: pong.o
	gcc -o pong pingpong-scheduler.o ppos.o queue.o

pong.o: ppos.o
	 gcc -c -Wall pingpong-scheduler.c

ppos.o: ppos.c
	gcc -c -Wall ppos.c

clean:
	 rm pingpong-scheduler.o ppos.o pong