#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "cpu.h"
#include "memory.h"
#include "stack.h"
#include "queue.h"
#include "logger_client.h"

#define CORE_SOCK "/tmp/sim_core.sock"
#define MEM_SIZE  10

static CPU   cpu;
static int  *memory;
static Stack stack;
static Queue queue;

static int run_command(const char *line, char *reply, size_t n) {
    char cmd[16] = {0};
    int a = 0, b = 0, c = 0;
    int got = sscanf(line, "%15s %d %d %d", cmd, &a, &b, &c);

    if (got < 1) { snprintf(reply, n, "ERR empty command"); return 0; }

    if (strcmp(cmd, "CPU") == 0) {
        if (got < 4) { snprintf(reply, n, "ERR usage: CPU op a b"); return 0; }
        if (a < 1 || a > 4) {
            snprintf(reply, n, "ERR invalid CPU operation %d", a);
            log_msg("ERROR", "Invalid CPU operation %d", a);
        } else if (a == 4 && c == 0) {
            snprintf(reply, n, "ERR divide by zero");
            log_msg("ERROR", "Divide by zero (%d / %d)", b, c);
        } else {
            int r = executeOperation(&cpu, a, b, c);
            snprintf(reply, n, "OK result=%d reg0=%d pc=%d", r, cpu.registers[0], cpu.programCounter);
            log_msg("INFO", "CPU op=%d a=%d b=%d result=%d", a, b, c, r);
        }
    }
    else if (strcmp(cmd, "MEMW") == 0) {
        if (got < 3) { snprintf(reply, n, "ERR usage: MEMW index value"); return 0; }
        if (a < 0 || a >= MEM_SIZE) {
            snprintf(reply, n, "ERR memory index %d out of range", a);
            log_msg("ERROR", "MEMW index %d out of range", a);
        } else {
            writeMemory(memory, a, b);
            snprintf(reply, n, "OK memory[%d]=%d", a, b);
            log_msg("INFO", "MEMW memory[%d]=%d", a, b);
        }
    }
    else if (strcmp(cmd, "MEMR") == 0) {
        if (got < 2) { snprintf(reply, n, "ERR usage: MEMR index"); return 0; }
        if (a < 0 || a >= MEM_SIZE) {
            snprintf(reply, n, "ERR memory index %d out of range", a);
            log_msg("ERROR", "MEMR index %d out of range", a);
        } else {
            int v = readMemory(memory, a);
            snprintf(reply, n, "OK memory[%d]=%d", a, v);
            log_msg("INFO", "MEMR memory[%d]=%d", a, v);
        }
    }
    else if (strcmp(cmd, "PUSH") == 0) {
        if (got < 2) { snprintf(reply, n, "ERR usage: PUSH value"); return 0; }
        if (isStackFull(&stack)) {
            snprintf(reply, n, "ERR stack overflow");
            log_msg("ERROR", "Stack overflow on PUSH %d", a);
        } else {
            push(&stack, a);
            snprintf(reply, n, "OK pushed %d", a);
            log_msg("INFO", "PUSH %d executed", a);
        }
    }
    else if (strcmp(cmd, "POP") == 0) {
        if (isStackEmpty(&stack)) {
            snprintf(reply, n, "ERR stack underflow");
            log_msg("ERROR", "Stack underflow on POP");
        } else {
            int v = pop(&stack);
            snprintf(reply, n, "OK popped %d", v);
            log_msg("INFO", "POP %d executed", v);
        }
    }
    else if (strcmp(cmd, "ENQ") == 0) {
        if (got < 2) { snprintf(reply, n, "ERR usage: ENQ value"); return 0; }
        if (isQueueFull(&queue)) {
            snprintf(reply, n, "ERR queue full");
            log_msg("ERROR", "Queue full on ENQUEUE %d", a);
        } else {
            enqueue(&queue, a);
            snprintf(reply, n, "OK enqueued %d", a);
            log_msg("INFO", "ENQUEUE %d executed", a);
        }
    }
    else if (strcmp(cmd, "DEQ") == 0) {
        if (isQueueEmpty(&queue)) {
            snprintf(reply, n, "ERR queue empty");
            log_msg("ERROR", "Queue empty on DEQUEUE");
        } else {
            int v = dequeue(&queue);
            snprintf(reply, n, "OK dequeued %d", v);
            log_msg("INFO", "DEQUEUE %d executed", v);
        }
    }
    else if (strcmp(cmd, "PING") == 0) {
        snprintf(reply, n, "OK pong");
    }
    else if (strcmp(cmd, "SHUTDOWN") == 0) {
        snprintf(reply, n, "OK shutting down");
        log_msg("INFO", "Core shutting down");
        return 1;
    }
    else {
        snprintf(reply, n, "ERR unknown command '%s'", cmd);
        log_msg("ERROR", "Unknown command '%s'", cmd);
    }
    return 0;
}

int main(void) {
    signal(SIGPIPE, SIG_IGN);

    initCPU(&cpu);
    memory = allocateMemory(MEM_SIZE);
    initStack(&stack);
    initQueue(&queue);

    if (log_init() < 0) fprintf(stderr, "[Core] Logger not running - continuing without logs\n");
    log_msg("INFO", "Core process started");

    int srv = socket(AF_UNIX, SOCK_STREAM, 0);
    if (srv < 0) { perror("socket"); return 1; }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, CORE_SOCK, sizeof(addr.sun_path) - 1);

    unlink(CORE_SOCK);
    if (bind(srv, (struct sockaddr *)&addr, sizeof(addr)) < 0) { perror("bind"); return 1; }
    if (listen(srv, 5) < 0) { perror("listen"); return 1; }

    printf("[Core] ready on %s\n", CORE_SOCK);
    int running = 1;

    while (running) {
        int cli = accept(srv, NULL, NULL);
        if (cli < 0) { perror("accept"); continue; }

        FILE *in  = fdopen(cli, "r");
        FILE *out = fdopen(dup(cli), "w");
        char line[256], reply[256];

        while (fgets(line, sizeof(line), in)) {
            line[strcspn(line, "\r\n")] = '\0';
            if (strcmp(line, "QUIT") == 0) {
                fprintf(out, "OK bye\n"); fflush(out);
                break;
            }
            if (run_command(line, reply, sizeof(reply))) running = 0;
            fprintf(out, "%s\n", reply);
            fflush(out);
            if (!running) break;
        }
        fclose(in);
        fclose(out);
    }

    log_close();
    freeMemory(memory);
    close(srv);
    unlink(CORE_SOCK);
    printf("[Core] shut down.\n");
    return 0;
}
