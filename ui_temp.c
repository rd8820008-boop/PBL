#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define CORE_SOCK "/tmp/sim_core.sock"

static FILE *to_core, *from_core;

static void send_cmd(const char *cmd) {
    fprintf(to_core, "%s\n", cmd);
    fflush(to_core);
    char reply[256];
    if (fgets(reply, sizeof(reply), from_core)) printf("Core: %s", reply);
    else printf("Core closed the connection.\n");
}

int main(void) {
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, CORE_SOCK, sizeof(addr.sun_path) - 1);
    if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Cannot connect to Core (is ./core_server running?)");
        return 1;
    }
    to_core   = fdopen(fd, "w");
    from_core = fdopen(dup(fd), "r");

    int choice;
    char cmd[128];
    while (1) {
        printf("\n--- Simulator UI ---\n"
               "1. CPU instruction\n2. Write memory\n3. Read memory\n"
               "4. Push\n5. Pop\n6. Enqueue\n7. Dequeue\n"
               "8. Shutdown everything\n0. Exit UI\nChoice: ");
        if (scanf("%d", &choice) != 1) break;
        if (choice == 0) { send_cmd("QUIT"); break; }

        int x, y, z;
        switch (choice) {
        case 1:
            printf("Operation (1=Add 2=Sub 3=Mul 4=Div): "); scanf("%d", &x);
            printf("First number: ");  scanf("%d", &y);
            printf("Second number: "); scanf("%d", &z);
            snprintf(cmd, sizeof(cmd), "CPU %d %d %d", x, y, z); send_cmd(cmd); break;
        case 2:
            printf("Index: "); scanf("%d", &x); printf("Value: "); scanf("%d", &y);
            snprintf(cmd, sizeof(cmd), "MEMW %d %d", x, y); send_cmd(cmd); break;
        case 3:
            printf("Index: "); scanf("%d", &x);
            snprintf(cmd, sizeof(cmd), "MEMR %d", x); send_cmd(cmd); break;
        case 4:
            printf("Value: "); scanf("%d", &x);
            snprintf(cmd, sizeof(cmd), "PUSH %d", x); send_cmd(cmd); break;
        case 5: send_cmd("POP"); break;
        case 6:
            printf("Value: "); scanf("%d", &x);
            snprintf(cmd, sizeof(cmd), "ENQ %d", x); send_cmd(cmd); break;
        case 7: send_cmd("DEQ"); break;
        case 8: send_cmd("SHUTDOWN"); return 0;
        default: printf("Invalid choice.\n");
        }
    }
    return 0;
}
