#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>

#define FIFO_PATH "/tmp/sim_logger.fifo"
#define LOG_FILE  "simulator.log"

int main(void) {
    unlink(FIFO_PATH);
    if (mkfifo(FIFO_PATH, 0666) < 0) { perror("mkfifo"); return 1; }

    FILE *log = fopen(LOG_FILE, "a");
    if (!log) { perror("fopen"); return 1; }

    printf("[Logger] ready (named pipe %s), writing to %s\n", FIFO_PATH, LOG_FILE);
    int running = 1;

    while (running) {
        int fd = open(FIFO_PATH, O_RDONLY);
        if (fd < 0) { perror("open"); break; }

        FILE *in = fdopen(fd, "r");
        char line[512];
        while (fgets(line, sizeof(line), in)) {
            line[strcspn(line, "\r\n")] = '\0';
            if (line[0] == '\0') continue;
            if (strcmp(line, "SHUTDOWN") == 0) { running = 0; break; }

            char ts[32];
            time_t now = time(NULL);
            strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M:%S", localtime(&now));
            fprintf(log, "[%s] %s\n", ts, line);
            fflush(log);
        }
        fclose(in);
    }

    fclose(log);
    unlink(FIFO_PATH);
    printf("[Logger] shut down.\n");
    return 0;
}
