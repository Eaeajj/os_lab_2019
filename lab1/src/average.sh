#!/bin/bash

if [ -n "$1" ]; then
    count=0
    sum=0
    while [ -n "$1" ]
    do 
        sum=$[$sum + $1]
        count=$[$count + 1]
        shift 
    done 

    bc <<< "scale=2;$sum/$count"
    echo $count
else 
    echo "no input"
fi