CFLAGS = -Wall -std=gnu99

all: main.c
	$(CC) $(CFLAGS) $^

clean:
	rm -f ./*.exe ./*.stackdump
