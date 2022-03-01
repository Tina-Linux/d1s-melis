#!/bin/bash

for f in `find ./../drivers/video -type f -name "*.a"`; 
    do 
        echo $f; 
        arm-melis-eabi-objdump -d $f > $1/$(basename $f).dis; 
    done
