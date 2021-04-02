all: contab prio preemp

contab: contab.o
	gcc -o contab pingpong-contab-prio.o ppos.o queue.o

contab.o: ppos.o
	gcc -c -Wall pingpong-contab-prio.c

prio: prio.o
	gcc -o prio pingpong-scheduler.o ppos.o queue.o

prio.o: ppos.o
	gcc -c -Wall pingpong-scheduler.c
	
preemp: preemp.o
	gcc -o preemp pingpong-preempcao.o ppos.o queue.o

preemp.o: ppos.o
	 gcc -c -Wall pingpong-preempcao.c

ppos.o: ppos-core-scheduler.c
	gcc -c -Wall ppos-core-scheduler.c

clean:
	 rm pingpong-scheduler.o pingpong-preempcao.o pingpong-contab-prio.o ppos.o 