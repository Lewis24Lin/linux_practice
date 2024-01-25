#!/usr/bin/bash

echo "$0 $1 $2 $3 $4"
echo "number of param: $#"
echo "whole param: $@"
echo "whole param: '$@'"
echo $@
echo "$1" "$2" "$3" "$4"
echo "$@"

shift
echo "$0 $1 $2 $3 $4"

shift 2
echo "$0 $1 $2 $3 $4"

echo "--------------------"

echo "if"
echo ""

#read -p "input str1: " s1
#read -p "input str2: " s2
s1=qwe
s2=asd

if [ "${s1}" == "qwe" ] && [ "${s2}" == "asd" ]; then
	echo "true 1"
elif [ "${s1}" == "QWE" -a "${s2}" == "ASD" ]; then
	echo "true 2"
else
	echo "false"
fi

echo "--------------------"

echo "case"
echo ""

#read -p "input a num: " a
a=1

case ${a} in
	"1")
	    echo "a = 1"
		;;
	"2")
		echo "a = 2"
		;;
	*)
		echo "others"
		;;
esac	
	
echo "--------------------"

echo "function"
echo ""

function sayHi(){
	echo "number of arguments: $#"
	echo "arguments: $@"
	echo "function name: ${0}" # ${0} don't get function name
	echo "Hi, ${1} ${2}"
}

name=lewis
sayHi ${name} lin

echo "--------------------"

echo "while, until"
echo ""

yn=yes

while [ "${yn}" != "yes" -a "${yn}" != "YES" ]
do
	read -p "Please input yes/YES to stop this program: " yn
done

yn=yes

until [ "${yn}" == "yes" -o "${yn}" == "YES" ]
do
	read -p "Please input yes/YES to stop this program: " yn
done

i=0
s=0

while [ "${i}" -le "10" ]
do
	s=$(($s+$i))
	i=$(($i+1))
done

echo ${i} ${s}

echo "--------------------"

echo "for"
echo ""

for animal in dog cat elephant
do
	echo "$animal"
done
echo ""

#users=$(cut -d ':' -f 1 /etc/passwd)
#echo ${users}

users="allan bella cindy"

for user in ${users}
do
	echo ${user}
done
echo ""

files=$(ls)

for file in $(ls)
do
	echo $file
done
echo ""

for n in $(seq 1 3)
do
	echo $n
done
echo ""

s=0
n=11
for ((i=0; i<${n}; i=i+1))
do
	s=$(($s+$i))	
done
echo ${s}

./shell2.sh
