#!/bin/bash
llc -march=x86 t.ll -o t.s
sleep 5
clang++ -m32 -g t.s -o a.out
