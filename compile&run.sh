#!/bin/sh
gcc src/master.c -lbmp -lm -o bin/master
gcc src/processA.c -lbmp -lncurses -lm -o bin/processA -lrt
gcc src/processB.c -lbmp -lncurses -lm -o bin/processB -lrt
./bin/master
