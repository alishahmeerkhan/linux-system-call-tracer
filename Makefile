all: main

child_tracking.o: child_tracking.c child_tracking.h
	gcc -c child_tracking.c -o child_tracking.o

external_tracking.o: external_tracking.c external_tracking.h
	gcc -c external_tracking.c -o external_tracking.o

logger.o: logger.c logger.h
	gcc -c logger.c -o logger.o
	
syscall_names.o: syscall_names.c syscall_names.h
	gcc -c syscall_names.c -o syscall_names.o

main.o: main.c external_tracking.h child_tracking.h
	gcc -c main.c -o main.o	

main: main.o external_tracking.o child_tracking.o logger.o syscall_names.o
	gcc main.o external_tracking.o child_tracking.o logger.o syscall_names.o -o main

clean:
	rm -f *.o main