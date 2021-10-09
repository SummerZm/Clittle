PROGS =	copy fcntl
CC = gcc
CFLAGS = -g -O2 -Wall
LIBS = 
CLEANFILES = *.o *.out 
BIN = ./bin/*

all:	${PROGS}

copy: copy.o	
		${CC} ${CFLAGS}  -o $@ $< ${LIBS}
		rm -f ${CLEANFILES}
		mv $@ ./bin/

fcntl: fcntl.o
		${CC} ${CFLAGS}  -o $@ $< ${LIBS}
		rm -f ${CLEANFILES}
		mv $@ ./bin/

clean:
		rm -f ${PROGS} ${CLEANFILES}
		rm -rf ${BIN}
