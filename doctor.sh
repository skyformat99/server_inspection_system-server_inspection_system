#!/bin/bash

eval $(cat ./log/system.log | cut -d " " -f 11-14 | awk '{printf("cpu["NR"]=%s mem["NR"]=%s disk["NR"]=%s\n", $1, $2, $3);} END {printf("count=%d", NR);}')
#echo $count
for ((i=1; i<=count; ++i)); do
    echo ${cpu[$i]} ${mem[$i]} ${disk[$i]}
done

