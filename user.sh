#!/bin/bash
User_sum=0

eval $(cat /etc/passwd	| awk -F: -v sum=0 ' {if($3 >= 1000 && $3 != 65534) {sum++}} END {printf("User_sum=%d\n", sum)}')

Active_sum=0
Active_usr=0
Active_num=0
eval $(last | sort | grep -v reboot | cut -d " " -f 1 | uniq -c | sort -r -n | head -n 3 | awk -v sum=0 ' {printf("Active_num[%d]=%s Active_user[%d]=%s\n", sum, $1, sum,$2);sum++} END{printf("Avtive_sum=%d\n", sum)}')

Cur_sum=0
Cur_user=0
Cur_pty=0
Cur_ip=0
eval $(w | sed -n '3,$p' | tr -s " " | cut -d " " -f 1-3 | awk -v sum=0 ' {printf("Cur_user[%d]=%s\tCur_pty[%d]=%s\tCur_ip[%d]=%s\n", sum, $1, sum, $2, sum, $3);sum++} END {printf("Cur_sum=%d\n", sum)}')

NowTime=`date +"%Y-%m-%d__%H:%M:%S"`
echo $NowTime
echo -e "total_u\tactive\tfreq\tcur_u\tpts\tip"

for ((i = 0; i <= $Avtive_sum || i <= $Cur_sum ; i++)) ;
do
	A1=""
	A2=""
	A3=""
	A4=""
	A5=""
	A6=""
	if [[ $i -eq 0 ]]; then
		 A1=$User_sum
	fi	
	if [[ $i -le $Avtive_sum ]]; then
			 A2=${Active_user[$i]}
			 A3=${Active_num[$i]}
	fi
	if [[ $i -le $Cur_sum ]]; then
			 A4=${Cur_user[$i]}
			 A5=${Cur_pty[$i]}
			 A6=${Cur_ip[$i]}
	fi
	echo -e "$A1\t$A2\t$A3\t$A4\t$A5\t$A6"

done
