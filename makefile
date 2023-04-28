runTree: main.o
	gcc -o runTree main.o

main.o: main.c
	gcc -c main.c

clean:
	rm -rf *.o runTree