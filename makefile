runTree: main.o Rtree.o
	gcc -o runTree main.o Rtree.o

Rtree.o: Rtree.c
	gcc -c Rtree.c

main.o: main.c
	gcc -c main.c

clean:
	rm -rf *.o runTree