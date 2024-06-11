#!/usr/bin/bash

export num=99

function func1(){
	echo "test ${num}"
	echo $1	
}
export -f func1

arr=(1 2 3 4 5)

echo "function call (parallel)"
parallel -j8 func1 ::: ${arr[0]}
echo "" 

exit 0
