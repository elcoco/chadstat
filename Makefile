CC = gcc
CFLAGS = -lasound -lcurl -Wall -lX11 -lmpdclient

# target: dependencies
# 	  action

dface: main.o blocks.o utils.o
	$(CC) main.o blocks.o utils.o $(CFLAGS) -o dface

main.o: main.c 
	$(CC) -c main.c

blocks.o: blocks.c blocks.h
	$(CC) -c blocks.c

utils.o: utils.c utils.h
	$(CC) -c utils.c

clean:
	rm *.o

install:
	cp -f dface ~/bin/apps
	chmod +x ~/bin/apps/dface
