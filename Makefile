CFLAGS = -Wall -std=gnu99

all: vehiclehud

vehiclehud: main.o obd.o elm327.o
	$(CC) $(CFLAGS) $^ -o vehiclehud

main.o: main.c
	$(CC) $(CFLAGS) -c $^

obd.o: obd.h obd.c
	$(CC) $(CFLAGS) -c $^

elm327.o: elm327.h elm327.c
	$(CC) $(CFLAGS) -c $^

clean:
	rm -f ./*.o ./*.gch ./*.exe ./*.stackdump
