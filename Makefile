# Makefile

CC=gcc
CFLAGS=
OBJS= store.o headquarter.o function.o
LIBS=
all :	add

add:	$(OBJS)
	$(CC) $(CFLAGS) -o phone.out $(OBJS) $(LIBS)

headquarter.o: headquarter.c
	$(CC) $(CFLAGS) -c headquarter.c -l phonehdr.h
store.0: store.c
	$(CC) $(CFLAGS) -c store.c -; phonehdr.h
function.o: function.c
	$(CC) $(CFLAGS) -c function.c -l phonehdr.h



clean:
	rm -f $(OBJS) phone.out core
