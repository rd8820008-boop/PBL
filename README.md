
# Multi-Process Simulator with IPC (PBL Team 1)

A simulator made of 3 separate processes written in C. They talk to each other using sockets (IPC).

## The 3 processes

| Process | File | What it does |
|---------|------|--------------|
| UI | `ui_temp.c` | Menu where the user chooses an action and sends commands to Core |
| Core | `core_server.c` | Runs the simulator: CPU operations, memory, stack and queue |
| Logger | `logger.c` | Receives messages from Core and saves them in `simulator.log` |

Supporting files: `cpu`, `memory`, `stack`, `queue` (the simulator parts) and `logger_client` (lets Core send log messages).

## How the processes talk

```
UI  --(socket /tmp/sim_core.sock)-->  Core  --(socket /tmp/sim_logger.sock)-->  Logger
```

- UI sends text commands (PUSH, POP, ENQ, DEQ, CPU, MEMW, MEMR, SHUTDOWN) to Core and prints Core's reply.
- Core runs the command, replies to the UI, and sends an INFO or ERROR message to the Logger.
- Logger writes each message with a timestamp into `simulator.log`.

## How to run

```
bash run.sh
```

This builds everything, starts the Logger and Core, and opens the UI menu.
Choose **8 (Shutdown everything)** to stop all processes. Then `simulator.log` is printed.

## Example

1. Push 10, then Pop (works), then Pop again (error: stack underflow)
2. Choose 8 to shut down
3. `simulator.log` shows the PUSH, POP, the underflow ERROR, and the shutdown

## Notes

- The UI is currently a simple terminal menu.
- Team members: (write your team's names here)
Rachel Dias
Anush
Inchara