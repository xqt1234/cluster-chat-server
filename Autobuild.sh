#!/bin/bash

set -e
set -u

if [ ! -d `pwd`/build ];then
    mkdir `pwd`/build
else
    rm -rf `pwd`/build/*
fi

rm  `pwd`/Server/*.log

cd `pwd`/build && cmake .. && make 

cd ..