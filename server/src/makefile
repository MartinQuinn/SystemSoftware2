CC=gcc
objects = main.o connection_handler.o
headers = main.c connection_handler.h
name = serverprogram

serverprogram : $(objects)
				$(CC) -o $(name) $(objects) -lm

main.o : main.c $(headers)
				$(CC) -c main.c

connection_handler.o : connection_handler.c
				$(CC) -c connection_handler.c

clean:
		rm $(name) $(objects)
