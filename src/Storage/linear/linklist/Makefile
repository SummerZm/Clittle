PROGS = storage
CC = gcc
CFLAGS = -g -O2 -Wall 
CLEANFILES = *.o
input = *.c

all: ${PROGS}
storage: storage.o linklist.o main.o
	${CC} ${CFLAGS} -o $@ storage.o main.o linklist.o
	rm -f ${CLEANFILES}
	mv $@ ./bin/

clean:
	rm -rf ./bin/*
	rm ${CLEANFILES}


#storage: *.c

#*.o:*.c
#	${CC} ${CFLAGS} $@ -o  $< 

#clean:
#	rm ${BIN}
