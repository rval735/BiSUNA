#!/bin/bash

GYMUDSPATH="../../gym-uds-api/"
ENVIRONMENTS=("MountainCar-v0" "NChain-v0" "Roulette-v0" "DuplicatedInput-v0" "Copy-v0")
GYMSERVERNAME="gym-uds-server.py"
TRIALS=100000

RUNBISUNA() {
    for i in {1..10}
    do
        echo "Run " $i
        echo "Discrete"
        time ./bisuna $TRIALS
        echo "<<<<>>>>"
        echo "Continuos"
        time ./suna $TRIALS
        echo "********"echo "********"
    done
}

for i in "${ENVIRONMENTS[@]}"
do
    echo "--------------------------------"
	echo "About to run: " $i " at " $(date)
    python3 $GYMUDSPATH$GYMSERVERNAME $i &
    PID=$!
    echo "Server Job number: " $PID
    sleep 2
    RUNBISUNA
    kill $PID
    echo "Killed: " $PID
    echo "About to start a new gym environment"
done