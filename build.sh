#!/bin/bash

IP=192.168.1.106
TID=0100E95004038000
CODE_NAME=bf2

make send -j8 IP=$IP TID=$TID CODE_NAME=$CODE_NAME
while true
do
	nc $IP 6969
	sleep 1
done
