#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCK_PATH "/tmp/sim_logger.sock"
#define LOG_FILE  "simulator.log"

int main(void) {
    int srv = socket(AF_UNIX, SOCK_STREAM, 0);
    if (srv < 0) { perror("socket"); return 1; }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCK_PATH, sizeof(addr.sun_path) - 1);

    unlink(SOCK_PATH);
    if (bind(srv, (struct sockaddr *)&addr, sizeof(addr)) < 0) { perror("bind"); return 1; }
    if (listen(srv, 5) < 0) { perror("listen"); return 1; }

    FILE *log = fopen(LOG_FILE, "a");
    if (!log) { perror("fopen"); return 1; }

    printf("[Logger] ready, writing to %s\n", LOG_FILE);
    int running = 1;

    while (running) {
        int cli = accept(srv, NULL, NULL);
        if (cli < 0) { perror("accept"); continue; }

        FILE *in = fdopen(cli, "r");
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
    close(srv);
    unlink(SOCK_PATH);
    printf("[Logger] shut down.\n");
    return 0;
}
