#!/bin/bash

./dralion4 > xx.ppm << EOF
11 0 -.11
10 0 -.10
500 500 0
15 15 0
`cat points`
EOF
