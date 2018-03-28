#!/bin/bash
#source ./piHealth.conf 


arr=(`free -m | head -n 2 | tail -n 1 | awk '{printf("%d %d %d", $2, $3, $7)}'`)
total=${arr[0]}
#echo $total
used=${arr[1]}
#echo $used
avaliable=${arr[2]}
#echo $avaliable

usePrecent1=`echo "scale=2;$used/$total" | bc`

sleep 2

NowTime=`date +"%T-%m-%d__%H:%M:%S"`
#echo $NowTime
arr=(`free -m | head -n 2 | tail -n 1 | awk '{printf("%d %d %d", $2, $3, $7)}'`)
total=${arr[0]}
#echo $total
used=${arr[1]}
#echo $used
avaliable=${arr[2]}
#echo $avaliable

usePrecent2=`echo "scale=2;$used/$total" | bc`
usePrecent1=`echo "scale=4;0.2*$usePrecent2+0.8*$usePrecent1" | bc`

echo "$NowTime $total $avaliable $usePrecent2 $usePrecent1" 
   
