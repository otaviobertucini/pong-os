all: driver

driver: ppos_disk.o ppos-core-task.o pingpong-disco1.o ppos-core-sched.o ppos-core-scheduler.o ppos-core-task-aux.o queue.o disk.o ppos-ipc-barrier.o ppos-ipc-mqueue.o ppos-ipc-mutex.o ppos-ipc-semaphore.o
	gcc -o driver ppos_disk.o ppos-core-task.o pingpong-disco1.o ppos-core-sched.o ppos-core-scheduler.o ppos-core-task-aux.o queue.o disk.o ppos-ipc-barrier.o ppos-ipc-mqueue.o ppos-ipc-mutex.o ppos-ipc-semaphore.o -lrt

ppos_disk.o: ppos_disk.c
	gcc -c -Wall ppos_disk.c -lrt

pingpong-disco1.o: pingpong-disco1.c
	gcc -c -Wall pingpong-disco1.c -lrt

ppos-core-scheduler.o: ppos-core-scheduler.c
	gcc -c -Wall ppos-core-scheduler.c -lrt

ppos-core-task-aux.o: ppos-core-task-aux.c
	gcc -c -Wall ppos-core-task-aux.c -lrt

clean:
	rm -f ppos-core-task-aux.o ppos-core-scheduler.o pingpong-disco1.o disk.o ppos_disk.o driver
