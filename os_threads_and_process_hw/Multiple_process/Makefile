all: mandel mandelseries

mandel: mandel.o bitmap.o
	gcc mandel.o bitmap.o -o mandel -lpthread

mandel.o: mandel.c
	gcc -Wall -g -c mandel.c -o mandel.o

bitmap.o: bitmap.c
	gcc -Wall -g -c bitmap.c -o bitmap.o

mandelseries.o: mandelseries.c
	gcc -Wall -g -c mandelseries.c -o mandelseries.o

clean:
	rm -f mandel.o bitmap.o mandel mandelseries.o mandelseries
