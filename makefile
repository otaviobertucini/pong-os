all: contab prio preemp

contab: contab.o
	gcc -o contab pingpong-contab-prio.o ppos-core-task-aux.o ppos-core-scheduler.o queue.o ppos-core-task.o ppos-core-sched.o 

contab.o: ppos-core-scheduler.o
	gcc -c -Wall pingpong-contab-prio.c

prio: prio.o
	gcc -o prio pingpong-scheduler.o ppos-core-scheduler.o queue.o ppos-core-task-aux.o ppos-core-task.o ppos-core-sched.o

prio.o: ppos-core-scheduler.o
	gcc -c -Wall pingpong-scheduler.c
	
preemp: preemp.o
	gcc -o preemp pingpong-preempcao.o ppos-core-scheduler.o queue.o ppos-core-task-aux.o ppos-core-task.o ppos-core-sched.o

preemp.o: ppos-core-scheduler.o
	 gcc -c -Wall pingpong-preempcao.c

ppos-core-scheduler.o: ppos-core-task-aux.o ppos-core-scheduler.c
	gcc -c -Wall ppos-core-scheduler.c

ppos-core-task-aux.o: ppos-core-task-aux.c
	gcc -c -Wall ppos-core-task-aux.c

clean:
	 rm -f pingpong-scheduler.o pingpong-preempcao.o pingpong-contab-prio.o ppos-core-scheduler.o prio preemp contab pong