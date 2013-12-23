#!/bin/bash

circumference()
{
RADIUS=$1
CENTERX=$2
CENTERY=$3
ANGLE=$4
X=`{ echo "scale=2" ; echo \
"${RADIUS}*c(${ANGLE})+${CENTERX}" ; } | bc -l`
Y=`{ echo "scale=2" ; echo \
"${RADIUS}*s(${ANGLE})+${CENTERY}" ; } | bc -l`
echo -n "${X} 0 ${Y}"
}

CTR=0
COUNT=0
LIMIT=
while test -z `echo "${LIMIT}" | grep '-' -`
do

FNAME=`printf 'p%03d.ppm' $CTR`

XY=`circumference 8 0 0 ${COUNT}`
XY2=`circumference 4 0 0 ${COUNT}`

./dralion4 > $FNAME << EOF
${XY}
${XY2}
100 100 0
10 10 0
`cat points`
EOF

echo $FNAME done

# echo "move to ${XY}"
# echo "${XY}"
# echo "circle rad 1.8"
COUNT=`{ echo "scale=2" ; echo \
"${COUNT}+0.01" ; } | bc -l`
LIMIT=`echo "6.28-${COUNT}" | bc -l`
CTR=`expr ${CTR} + 1`
done
