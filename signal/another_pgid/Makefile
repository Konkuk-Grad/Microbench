CC=gcc
CFLAGS = 
LDFLAGS = -lrt

signal_ping.out: signal_ping.o
	$(CC) -o signal_ping.out signal_ping.o

signal_ping.o: signal_ping.c
	$(CC) -c -o signal_ping.o signal_ping.c -DCLOCK_GETTIME

signal_pong.out: signal_pong.o
	$(CC) -o signal_pong.out signal_pong.o

signal_pong.o: signal_pong.c
	$(CC) -c -o signal_pong.o signal_pong.c -DCLOCK_GETTIME