#!/bin/bash
#source ./piHealth.conf
NowTime=`date +"%Y-%m-%d__%H:%M:%S"`
#echo $NowTime

totalsize=`df -m | grep -v tmpfs | tail -n +2 | awk -v sum=0 '{sum=sum+$2} END {printf("%d\n",sum)}'`
#echo $totalsize

eval $(df -m | grep -v tmpfs | grep -v udev |tail -n +2 | awk -v cnt=0 -v sum=0 -v disuse=0 '{sum=sum+$2;disuse=disuse+$4;printf("sizes[%d]=%d avaliable[%d]=%d precents[%d]=%s paths[%d]=%s\n", NR, $2, NR, $4, NR, $5, NR, $6);} END {printf("cnt=%d\n", NR);printf("sum=%d disuse=%d\n", sum, disuse)}')
#eval将打印出来的东西转换为赋值语句
#echo ${sizes[*]} ${useds[*]} ${precents[*]} ${paths[*]}
for ((i=1;i<=$cnt;++i));do
    echo "$NowTime 1 ${paths[i]}  ${sizes[i]} ${avaliable[i]} ${precents[i]}"
done
precent=`echo "scale=2;100-$disuse/$sum*100" | bc`
echo "$NowTime 0 disk $sum $disuse $precent%" 
 
