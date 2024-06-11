#!/usr/bin/bash


n1=1

if [ $n1 -gt 0 -a $n1 -lt 10 ];then
	echo "-a in []"
fi
if [[ $n1 -gt 0 && $n1 -lt 10 ]];then
	echo "&& in [[]]"
fi


s1="qwe asd"

if [ "$s1" == "qwe asd" ];then
	echo "\" \" for string variable in []"
fi
if [[ $s1 == "qwe asd" ]];then
	echo "no \" \" for string variable in [[]]"
fi	

a="["
if $a $n1 -eq 1 ];then
	echo "A"
fi
a="[["
if $a $n1 -eq 1 ]];then
	echo "B"
fi
