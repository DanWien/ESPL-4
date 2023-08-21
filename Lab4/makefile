all: myshell mypipe

myshell:
	gcc -m32 -g -Wall -o myshell myshell.c

mypipe:
	gcc -m32 -g -Wall -o mypipe mypipe.c

.PHONY: clean
clean:
	rm -f myshell.o myshell mypipe