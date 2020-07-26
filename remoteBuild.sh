#!/bin/bash

if [ "$*" = "" ];then
    echo "usage: remoteBuild.sh [commit message]"
    exit 9
fi  

git commit -am "$*" && git push 
ssh morpheus "cd /home/wryan/projects/udd_rpi_lib; git pull && make install"

