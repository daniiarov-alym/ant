ant: ant.o
	mkdir -p build
	gcc -o build/ant ant.o -lpthread -lncurses
	
ant.o:
	gcc -c -g ant.c

clean:
	rm *.o		