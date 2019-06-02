TARGET = testfilesystem
LIBS = -lm
CC = gcc
CFLAGS = -g -Wall

.PHONY: default all clean


default: $(TARGET)
all: default

OBJS=$(patsubst %.c, %.o, $(wildcard *.c))

HEADS=$(wildcard *.h)


%.o: %.c $(HEADS)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJS)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -Wall $(LIBS) -o $@
clean:
	rm -f *.o 
	rm -f *.sd
	rm -f $(TARGET)
