CFLAGS = -Wall -std=gnu99

all: vehiclehud

vehiclehud: main.o obd.o
	$(CC) $(CFLAGS) $^ -o vehiclehud

main.o: main.c
	$(CC) $(CFLAGS) -c $^

obd.o: obd.h obd.c
	$(CC) $(CFLAGS) -c $^

clean:
	rm -f ./*.o ./*.gch ./*.exe ./*.stackdump
