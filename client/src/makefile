CC=gcc
objects = main.o
headers = main.c
name = clientprogram

clientprogram : $(objects)
		$(CC) -o $(name) $(objects) -lm

main.o : main.c $(headers)
		$(CC) -c main.c

debug : $(cfiles)
		$(CC) -g $(cfiles) -lm -o $(name)

clean:
		rm $(name) $(objects)