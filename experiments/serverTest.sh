#! /bin/bash

# format: $1 = number of mice $2 = number of elephants $3 = starting port number
n=$(($1 + $2))
port=$(($3))
for ((i=1; i<=n; i++, port++));
do
   ./server $port > results/router/flows/experiment$1-$2-$i &
done
