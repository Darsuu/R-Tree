runTree:  Rtree.o
	gcc -o runTree Rtree.o

Rtree.o: Rtree.c
	gcc -c Rtree.c

clean:
	rm -rf *.o runTree