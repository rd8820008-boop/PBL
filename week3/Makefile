CC = gcc
CFLAGS = -Wall -Wextra -std=c99
SRC = main.c metrics.c fcfs.c strf.c round_robin.c gantt.c

scheduler: $(SRC) scheduler.h
	$(CC) $(CFLAGS) -o scheduler $(SRC)

clean:
	rm -f scheduler