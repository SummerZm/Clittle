CC=gcc
CFLAGS=-Wall -g -O0
SRC=*.c
OBJS=target-arm
$(OBJS):$(SRC)
	$(CC) $(CFLAGS) -o $@ $^
clean:
	$(RM) $(OBJS) .*.sw?
