#!/bin/bash
llc -march=x86 t.ll -o t.s
sleep 5
gcc -m32 -g t.s jlstdlib.cpp -o a.out
