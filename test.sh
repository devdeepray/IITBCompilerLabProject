#!/bin/bash
 rm -f op.asm
 rm -f machine
 ./gompiler < $1 > op.asm
 g++ -m32 -Wno-write-strings machine.cc -o machine
 ./machine
