tree1: tree1.o lifo.o fifo.o
	gcc -Wall -g fifo.o tree1.o lifo.o -o tree1 -fsanitize=address	
tree1.o: tree1.c fifo.h
	gcc -Wall -g -c project3.c
lifo.o: lifo.c lifo.h
	gcc -Wall -g -c lifo.c
fifo.o: fifo.h fifo.c
	gcc -Wall -g -c fifo.c
clean:
	rm -fifo.o -lifo.o -tree1.o
