all: deadlock

deadlock: main.c banker.c detect.c deadlock.h
	gcc
Wall -o deadlock main.c banker.c detect.c

clean:
	rm -f deadlock
