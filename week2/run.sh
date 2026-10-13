#!/bin/bash
make || exit 1
./logger &
sleep 0.5
./core_server &
sleep 0.5
./ui_temp
wait
echo "---- simulator.log ----"
cat simulator.log
