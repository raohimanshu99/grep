CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = filesearch
OBJS = main.o hashtable.o filemanager.o wordparser.o search.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

main.o: main.c hashtable.h filemanager.h wordparser.h search.h
	$(CC) $(CFLAGS) -c main.c

hashtable.o: hashtable.c hashtable.h
	$(CC) $(CFLAGS) -c hashtable.c

filemanager.o: filemanager.c filemanager.h hashtable.h wordparser.h
	$(CC) $(CFLAGS) -c filemanager.c

wordparser.o: wordparser.c wordparser.h
	$(CC) $(CFLAGS) -c wordparser.c

search.o: search.c search.h hashtable.h filemanager.h wordparser.h
	$(CC) $(CFLAGS) -c search.c

clean:
	rm -f $(OBJS) $(TARGET)

rebuild: clean all

.PHONY: all clean rebuild
