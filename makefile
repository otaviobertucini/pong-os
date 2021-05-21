all: driver

driver: ppos_disk.o pingpong-disco1.o queue.o disk.o ppos-all.o disk.o ppos-core-task-aux.o
	gcc -o driver pingpong-disco1.o ppos_disk.o queue.o disk.o ppos-all.o ppos-core-task-aux.o -lrt

driver2: ppos_disk.o pingpong-disco2.o queue.o disk.o ppos-all.o disk.o ppos-core-task-aux.o
	gcc -o driver2 pingpong-disco2.o ppos_disk.o queue.o disk.o ppos-all.o ppos-core-task-aux.o -lrt

ppos_disk.o: ppos_disk.c
	gcc -c -Wall ppos_disk.c -lrt

pingpong-disco1.o: pingpong-disco1.c
	gcc -c -Wall pingpong-disco1.c -lrt

pingpong-disco2.o: pingpong-disco2.c
	gcc -c -Wall pingpong-disco2.c -lrt

ppos-core-scheduler.o: ppos-core-scheduler.c
	gcc -c -Wall ppos-core-scheduler.c -lrt

ppos-core-task-aux.o: ppos-core-task-aux.c
	gcc -c -Wall ppos-core-task-aux.c -lrt

disk.o: disk.c
	gcc -c -Wall disk.c -lrt

clean:
	rm -f ppos-core-task-aux.o ppos-core-scheduler.o pingpong-disco1.o disk.o ppos_disk.o driver
