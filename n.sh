#!/bin/bash

gcc -o dralion4 dralion4.c -lm
./c2.sh
ffmpeg -y -r 5 -i p%03d.ppm -r 5 out2.avi
mplayer -loop 0 -fs out2.avi
