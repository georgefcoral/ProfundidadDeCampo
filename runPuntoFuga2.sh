#!/bin/bash


if [ -z $1]
then
    echo "Arguments 1 is empty"
    exit 0
fi

if [ -z $2]
then
    echo "Arguments 2 is empty"
    exit 0
fi

if [ -z $3]
then
    echo "Arguments 3 is empty"
    exit 0
fi

make 

./PuntoDeFuga2 $1 $2 $3

cp tracking.m pTracking/
cp puntoDeFugaH.m pTracking/
