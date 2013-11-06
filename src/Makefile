CFLAGS += -std=gnu99 -Wall

all: vhudpi avrtest

vhudpi: vhudpi.c
avrtest: avrtest.c

format: $(src)
	indent -linux *.c

clean:
	rm -rf *.exe *~
