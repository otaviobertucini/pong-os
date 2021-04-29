all: driver

driver: ppos_disk.o ppos-core-task.o pingpong-disco1.o ppos-core-scheduler.o ppos-core-task-aux.o disk.o
	gcc -o driver pingpong-disco1.o ppos_disk.o ppos-core-task.o queue.o ppos-core-scheduler.o ppos-core-task-aux.o disk.o -lrt

ppos_disk.o: ppos_disk.c
	gcc -c -Wall ppos_disk.c -lrt

pingpong-disco1.o: pingpong-disco1.c
	gcc -c -Wall pingpong-disco1.c -lrt

ppos-core-scheduler.o: ppos-core-scheduler.c
	gcc -c -Wall ppos-core-scheduler.c -lrt

ppos-core-task-aux.o: ppos-core-task-aux.c
	gcc -c -Wall ppos-core-task-aux.c -lrt
