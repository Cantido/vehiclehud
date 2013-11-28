CFLAGS += -std=gnu99 -Wall

all: vhudpi avrtest

avr.o: avr.h
obd.o: obd.h

vhudpi: vhudpi.c avr.o obd.o
avrtest: avrtest.c avr.o

install: vhudpi
	mv vhudd /etc/init.d/vhudd
	chmod 755 /etc/init.d/vhudd
	sudo update-rc.d vhudd defaults

uninstall:
	/etc/init.d/vhudd stop
	update-rc.d -f vhudd remove
	rm -f /etc/init.d/vhudd

format: $(src)
	indent -linux *.c
	
clean:
	rm -rf *.exe *~
