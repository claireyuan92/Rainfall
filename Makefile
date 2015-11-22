#----- Students, modify CC and CFLAGS as appropriate in your environment
CC = gcc
CFLAGS = 

LIB = -lm -lc 

UNITFILES = 

INCLUDE = 

#------------ EXECUTABLE FILES : MOSTLY EXPERIMENTS ---------------

rainfall : rainfall.o $(UNITFILES)
	$(CC) $(CFLAGS)  -o rainfall rainfall.o $(UNITFILES) $(LIB)

clean:
	rm -f rainfall *.o

rainfall.o : rainfall.c
	$(CC) $(CFLAGS) -c rainfall.c $(INCLUDE)


