#! /bin/bash

for i in $(ps aux | grep "server 9*" | cut -d " " -f 3)
do
    kill -9 $i
done

