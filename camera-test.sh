#! /bin/sh

CAMS="1,14 2,15 3,11 4,10 5,12 6,16 7,17 8,18 9,19 10,20"
FILENAME=".cam-results"

if [ "x$#" == "x0" ]; then
	rm -f $FILENAME

	for cam in $CAMS; do
		$0 $cam >> $FILENAME &
	done

	for job in `jobs -p`; do
		wait $job
	done

	cat $FILENAME | sort -k 2n
	rm -f $FILENAME
else
	for cam in $@; do
		data=`echo $cam | tr ',' ' '`
		id=`echo $data | awk '{print $1}'`
		id=`printf %2d $id`
		ip="192.168.1.`echo $data | awk '{print $2}'`"
		ping -c 1 -W 1 $ip > /dev/null
		if [ "x$?" == "x0" ]; then
			output="\033[1;32m[ UP ]\033[0m"
		else
			output="\033[1;31m[DOWN]\033[0m"
		fi
		echo -e "\033[1;37mCamera $id \033[0;37m($ip):\t $output"
	done
fi
