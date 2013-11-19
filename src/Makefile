CFLAGS += -std=gnu99 -Wall

all: vhudpi avrtest

vhudpi: vhudpi.c
avrtest: avrtest.c

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
