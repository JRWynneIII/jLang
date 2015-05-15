#!/bin/bash
chmod 755 t.ll
llc -march=ppc64le t.ll -o t.s
sleep 5
gcc -g t.s -o a.out
