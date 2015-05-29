#!/bin/bash
chmod 755 t.ll
gcc ../src/stdio.cpp -lm -c -o stdio.o
llc -march=ppc64le t.ll -o t.s
sleep 5
gcc -g stdio.o t.s -lm -o a.out
