#!/bin/bash

# Length of input file string
inp=$1
n=${#inp} 

# Output file name
out=${inp:0:(n-4)}

# Compile
g++ $inp -o $out -I/usr/include -L/usr/lib -lglut -lGL -lGLU -lX11

