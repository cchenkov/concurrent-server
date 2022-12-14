#!/bin/bash

N=${1:-10}

RANDOM=$$

for i in `seq $N`
do
    echo $RANDOM
done
