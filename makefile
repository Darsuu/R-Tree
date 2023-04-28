runTree: test.o
	gcc -o runTree test.o

test.o: test.c
	gcc -c test.c

clean:
	rm -rf *.o runTree