#!/bin/bash

set -e
set -u

if [ ! -d `pwd`/build ];then
    mkdir `pwd`/build
else
    rm -rf `pwd`/build/*
fi

if ls `pwd`/Server/*.log >/dev/null 2>&1;then
    rm  `pwd`/Server/*.log
fi

cd `pwd`/build && cmake .. && make 

cd ..