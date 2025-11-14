#!/bin/bash

set -e
set -u

if [ ! -d `pwd`/build ];then
    mkdir `pwd`/build
else
    rm -rf `pwd`/build/*
fi

if ls `pwd`/src/client/*.log >/dev/null 2>&1;then
    rm  `pwd`/src/client/*.log
fi

if ls `pwd`/src/server/*.log >/dev/null 2>&1;then
    rm  `pwd`/src/server/*.log
fi
cd `pwd`/build && cmake .. && make 

cd ..