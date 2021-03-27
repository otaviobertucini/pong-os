all: pong

pong: pong.o
	gcc -o pong pingpong-contab-prio.o ppos.o queue.o

pong.o: ppos.o
	 gcc -c -Wall pingpong-contab-prio.c
s
ppos.o: ppos.c
	gcc -c -Wall ppos.c

clean:
	 rm pingpong-contab-prio.o ppos.o pong
