#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "logger_client.h"

#define SOCK_PATH "/tmp/sim_logger.sock"

static int log_fd = -1;

int log_init(void) {
    signal(SIGPIPE, SIG_IGN);
    log_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (log_fd < 0) return -1;

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCK_PATH, sizeof(addr.sun_path) - 1);

    if (connect(log_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        close(log_fd);
        log_fd = -1;
        return -1;
    }
    return 0;
}

void log_msg(const char *level, const char *fmt, ...) {
    if (log_fd < 0) return;
    char body[400], line[450];
    va_list args;
    va_start(args, fmt);
    vsnprintf(body, sizeof(body), fmt, args);
    va_end(args);
    snprintf(line, sizeof(line), "%s: %s\n", level, body);
    if (write(log_fd, line, strlen(line)) < 0) {
        close(log_fd);
        log_fd = -1;
    }
}

void log_close(void) {
    if (log_fd >= 0) {
        if (write(log_fd, "SHUTDOWN\n", 9) < 0) { }
        close(log_fd);
        log_fd = -1;
    }
}
