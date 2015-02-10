#!/bin/bash
llc -march=x86 t.ll -o t.s
sleep 5
gcc -g t.s -o a.out
