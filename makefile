all: pong

pong: pong.o
	gcc -o pong pingpong-scheduler.o ppos.o

pong.o: ppos.o
	 gcc -c pingpong-scheduler.c

ppos.o: ppos.c
	gcc -c ppos.c
     
clean:
	 rm pingpong-scheduler.o ppos.o pong