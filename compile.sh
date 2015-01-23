#!/bin/bash
llc t.ll -o t.s
sleep 5
gcc -g t.s -o a.out
