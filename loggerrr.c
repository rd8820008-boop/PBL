/* net.h - tiny portable TCP helper (Windows Winsock + Linux/macOS) */
#ifndef NET_H
#define NET_H

#include <stdio.h>
#include <string.h>

#ifdef _WIN32
  #include <winsock2.h>
  #include <ws2tcpip.h>
  typedef SOCKET sock_t;
  #define SOCK_INVALID INVALID_SOCKET
  #define sock_close closesocket
#else
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <unistd.h>
  typedef int sock_t;
  #define SOCK_INVALID (-1)
  #define sock_close close
#endif

#define CORE_PORT      5001
#define LOGGER_PORT    5002
#define LOCALHOST      "127.0.0.1"
#define NET_LINE_MAX   512

static int net_init(void) {
#ifdef _WIN32
    WSADATA w;
    return WSAStartup(MAKEWORD(2, 2), &w) == 0 ? 0 : -1;
#else
    return 0;
#endif
}

static void net_cleanup(void) {
#ifdef _WIN32
    WSACleanup();
#endif
}

/* Server side: bind + listen on 127.0.0.1:port */
static sock_t net_listen(int port) {
    sock_t s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == SOCK_INVALID) return SOCK_INVALID;

    int yes = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (const char *)&yes, sizeof(yes));

    struct sockaddr_in a;
    memset(&a, 0, sizeof(a));
    a.sin_family = AF_INET;
    a.sin_addr.s_addr = inet_addr(LOCALHOST);
    a.sin_port = htons((unsigned short)port);

    if (bind(s, (struct sockaddr *)&a, sizeof(a)) != 0 || listen(s, 8) != 0) {
        sock_close(s);
        return SOCK_INVALID;
    }
    return s;
}

/* Client side: connect to host:port */
static sock_t net_connect(const char *host, int port) {
    sock_t s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == SOCK_INVALID) return SOCK_INVALID;

    struct sockaddr_in a;
    memset(&a, 0, sizeof(a));
    a.sin_family = AF_INET;
    a.sin_addr.s_addr = inet_addr(host);
    a.sin_port = htons((unsigned short)port);

    if (connect(s, (struct sockaddr *)&a, sizeof(a)) != 0) {
        sock_close(s);
        return SOCK_INVALID;
    }
    return s;
}

/* Send one text line (adds '\n'). Returns 0 on success, -1 on error. */
static int net_send_line(sock_t s, const char *line) {
    char buf[NET_LINE_MAX + 2];
    size_t n = strlen(line);
    if (n > NET_LINE_MAX) n = NET_LINE_MAX;
    memcpy(buf, line, n);
    buf[n++] = '\n';

    size_t sent = 0;
    while (sent < n) {
        int r = (int)send(s, buf + sent, (int)(n - sent), 0);
        if (r <= 0) return -1;
        sent += (size_t)r;
    }
    return 0;
}

/* Read one line (without '\n' / '\r'). Returns length, or -1 if the peer closed. */
static int net_recv_line(sock_t s, char *out, int cap) {
    int n = 0;
    char c;
    while (n < cap - 1) {
        int r = (int)recv(s, &c, 1, 0);
        if (r <= 0) return -1;
        if (c == '\n') break;
        if (c != '\r') out[n++] = c;
    }
    out[n] = '\0';
    return n;
}

#endif
#include <stdlib.h>
#include <time.h>
#include "net.h"

#define MAX_CLIENTS 8
#define LOG_FILE "simulator.log"

static void timestamp(char *out, size_t cap) {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    strftime(out, cap, "%Y-%m-%d %H:%M:%S", tm);
}

static void write_log(FILE *f, const char *raw) {
    char ts[32], src[64] = "UNKNOWN";
    const char *msg = raw;

    const char *bar = strchr(raw, '|');
    if (bar) {
        size_t n = (size_t)(bar - raw);
        if (n >= sizeof(src)) n = sizeof(src) - 1;
        memcpy(src, raw, n);
        src[n] = '\0';
        msg = bar + 1;
    }
    timestamp(ts, sizeof(ts));
    printf("[%s] [%s] %s\n", ts, src, msg);
    fprintf(f, "[%s] [%s] %s\n", ts, src, msg);
    fflush(stdout);
    fflush(f);
}

int main(void) {
    if (net_init() != 0) { fprintf(stderr, "Winsock init failed\n"); return 1; }

    FILE *f = fopen(LOG_FILE, "a");
    if (!f) { perror("fopen " LOG_FILE); return 1; }

    sock_t srv = net_listen(LOGGER_PORT);
    if (srv == SOCK_INVALID) {
        fprintf(stderr, "Logger: cannot listen on port %d (already running?)\n", LOGGER_PORT);
        return 1;
    }
    printf("Logger listening on %s:%d  (writing to %s)\n", LOCALHOST, LOGGER_PORT, LOG_FILE);
    write_log(f, "LOGGER|started");

    sock_t clients[MAX_CLIENTS];
    for (int i = 0; i < MAX_CLIENTS; i++) clients[i] = SOCK_INVALID;

    int running = 1;
    while (running) {
        fd_set rd;
        FD_ZERO(&rd);
        FD_SET(srv, &rd);
        int maxfd = (int)srv;
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i] != SOCK_INVALID) {
                FD_SET(clients[i], &rd);
                if ((int)clients[i] > maxfd) maxfd = (int)clients[i];
            }
        }

        if (select(maxfd + 1, &rd, NULL, NULL, NULL) < 0) break;

        if (FD_ISSET(srv, &rd)) {
            sock_t c = accept(srv, NULL, NULL);
            if (c != SOCK_INVALID) {
                int placed = 0;
                for (int i = 0; i < MAX_CLIENTS; i++) {
                    if (clients[i] == SOCK_INVALID) { clients[i] = c; placed = 1; break; }
                }
                if (!placed) sock_close(c);
            }
        }

        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i] == SOCK_INVALID || !FD_ISSET(clients[i], &rd)) continue;

            char line[NET_LINE_MAX];
            if (net_recv_line(clients[i], line, sizeof(line)) < 0) {
                sock_close(clients[i]);
                clients[i] = SOCK_INVALID;
                continue;
            }
            if (strcmp(line, "QUIT") == 0) { running = 0; break; }
            write_log(f, line);
        }
    }

    write_log(f, "LOGGER|stopped");
    for (int i = 0; i < MAX_CLIENTS; i++)
        if (clients[i] != SOCK_INVALID) sock_close(clients[i]);
    sock_close(srv);
    fclose(f);
    net_cleanup();
    return 0;
}
