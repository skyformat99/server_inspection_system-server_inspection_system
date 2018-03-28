#!/bin/bash
NowTime=`date +"%Y-%m-%d__%H:%M:%S"`
#echo $NowTime

eval $(uname -a | awk '{printf("os=%s hostname=%s banben=%s\n", $15, $2, $3);}'
)

#echo "$os $hostname $banben"

RunTime=`cat /proc/uptime | cut -d " " -f 1`
#echo $RunTime

eval $(uptime | tr -s -t "," " " | cut -d " " -f 11-13 | awk -v average=0 '{average=average+$1+$2+$3;} END {average=average/3.0;printf("average=%f", average);}')
#echo $average
average=`echo "$average*100" | bc`
#echo "$average%"

#磁盘信息
eval $(df -m | grep -v tmpfs | grep -v udev | tail -n +2 | awk -v DiskTotal=0 -v DiskusedPrecent=0 '{DiskTotal=DiskTotal+$2; DiskusedPrecent=DiskusedPrecent+$3;} END {printf("DiskTotal=%d DiskusedPrecent=%d", DiskTotal, DiskusedPrecent);}')
#echo "$DiskTotal $DiskusedPrecent"

DiskusedPrecent=`echo "scale=4;${DiskusedPrecent}/${DiskTotal}*100" | bc`
#echo "$DiskTotal $DiskusedPrecent%"

#内存信息
eval $(free -m | head -n 2 | tail -n 1 | awk '{MemTotal=$2;MemusedPrencent=$3;printf("MemTotal=%d MemusedPrencent=%d", MemTotal, MemusedPrencent);}')
MemusedPrencent=`echo "scale=4;$MemusedPrencent/$MemTotal*100" | bc`
#echo "$MemTotal $MemusedPrencent%"

#CPU信息
CpuTemp=`cat /sys/class/thermal/thermal_zone0/temp`
CpuTemp=`echo "scale=2;$CpuTemp/1000" | bc`
#echo $CpuTemp

CpuWarnLevel="normal"
if [[ `echo $CpuTemp '>=' 90 | bc -l` = 1 ]]; then
    CpuWarnLevel="warning"
elif [[ `echo $CpuTemp '>=' 80 | bc -l` = 1 ]]; then
    CpuWarnLevel="note"
fi

DiskWarnLevel="normal"
if [[ `echo $DiskusedPrecent '>=' 80 | bc -l` = 1 ]]; then
    DiskWarnLevel="warning"
elif [[ `echo $DiskusedPrecent '>=' 70 | bc -l` = 1 ]]; then
    DiskusedPrecent="note"
fi

MemWarnLevel="normal"
if [[ `echo $MemusedPrencent '>=' 80 | bc -l` = 1 ]]; then
    MemWarnLevel="warning"
elif [[ `echo $MemusedPrencent '>=' 70 | bc -l` = 1 ]]; then
    MemWarnLevel="note"
fi

#echo "$CpuWarnLevel $DiskWarnLevel $MemWarnLevel"

echo "$NowTime $os $banben $RunTime $average% $DiskTotal $DiskusedPrecent% $MemTotal $MemusedPrencent% $CpuTemp℃  $DiskWarnLevel $MemWarnLevel $CpuWarnLevel"
