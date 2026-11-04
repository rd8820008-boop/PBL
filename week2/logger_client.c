#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include "logger_client.h"

#define FIFO_PATH "/tmp/sim_logger.fifo"

static int log_fd = -1;

int log_init(void) {
    signal(SIGPIPE, SIG_IGN);
    log_fd = open(FIFO_PATH, O_WRONLY | O_NONBLOCK);
    if (log_fd < 0) return -1;
    int flags = fcntl(log_fd, F_GETFL);
    fcntl(log_fd, F_SETFL, flags & ~O_NONBLOCK);
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
