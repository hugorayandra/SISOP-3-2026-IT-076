CC = gcc
CFLAGS = -Wall -pthread
LDFLAGS = -lrt

all: server client

server:
	$(CC) $(CFLAGS) orion.c -o orion $(LDFLAGS)

client:
	$(CC) $(CFLAGS) eternal.c -o eternal $(LDFLAGS)

clean:
	rm -f orion eternal

clear_ipc:
	ipcs -m | grep 0x00001234 | awk '{print $$2}' | xargs -r ipcrm -m
	ipcs -q | grep 0x00005678 | awk '{print $$2}' | xargs -r ipcrm -q
