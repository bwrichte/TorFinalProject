#! /bin/bash

# format:  $1 = ip $2 = port number $3 = small file $4 = times to execute
n=$(($4))
for ((i=1; i<=n; i++));
do
   ./client $1 $2 < $3 
done
