CC := gcc
CFLAGS := -Wall -Wextra
LDFLAGS := -lm

all: server client

server: server.c
	$(CC) $(CFLAGS) $< -o $@

client: client.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

clean:
	rm -f server client