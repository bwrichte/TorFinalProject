#! /bin/bash

# format: $1 = number of mice $2 = number of elephants $3 = ip $4 = starting port number $5 = small file $6 = large file
n=$(($2))
port=$(($4))
for ((i=1; i<=n; i++, port++));
do
   ./client $3 $port < $6 &
done

n=$(($1))
for ((i=1; i<=n; i++, port++));
do
   ./launchClient.sh $3 $port $5 10000 &
done
