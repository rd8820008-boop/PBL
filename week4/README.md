# Interactive Deadlock Simulation System (C + Web GUI)

## Files
- deadlock.h : shared System data structure and function declarations
- banker.c   : Banker's Algorithm (Need matrix, safety check, resource request)
- detect.c   : Deadlock detection and recovery (process termination)
- main.c     : reads a command and the matrices, prints the result as JSON
- server.py  : small web server that connects the GUI to the C program
- index.html : GUI (matrices, Resource Allocation Graph, state colours, utilization, log)
- Makefile   : builds the C program

## How to run (GitHub Codespaces)
    cd week4
    make
    python3 server.py
Then open port 5000 in the browser.

## Features
- Create processes and resources
- Enter Allocation and Maximum matrices, Need matrix is calculated automatically
- Request resources using the Banker's Algorithm
- Shows SAFE, UNSAFE or DEADLOCKED state with the safe sequence
- Deadlock detection and recovery by process termination
- Resource Allocation Graph and resource utilization