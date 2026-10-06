CC = gcc
CFLAGS = -Wall -pthread
MODULES = cpu.c memory.c stack.c queue.c

all: logger core_server ui_temp core_standalone

logger: logger.c
	$(CC) $(CFLAGS) logger.c -o logger

core_server: core_server.c logger_client.c $(MODULES)
	$(CC) $(CFLAGS) core_server.c logger_client.c $(MODULES) -o core_server

ui_temp: ui_temp.c
	$(CC) $(CFLAGS) ui_temp.c -o ui_temp

core_standalone: core.c $(MODULES)
	$(CC) $(CFLAGS) core.c $(MODULES) -o core_standalone

clean:
	rm -f logger core_server ui_temp core_standalone simulator.log /tmp/sim_logger.sock /tmp/sim_core.sock
