#!/bin/bash

threads=(1 4 8 16)
N1=235
N2=1575
Compiler=gcc

if [ ! -d "./build_$Compiler" ]; then #if build doesn't exist, then create it
	mkdir build_$Compiler
fi
cd build_$Compiler

# compile all targets
$Compiler -O3 -Wall -Werror -o lab1-seq ../lab1.c -lm
if [ $? -eq 0 ];then
    echo 'Compile seq success!!!'
    programs=("${programs[@]}" lab1-seq)
fi

for loop in ${threads[@]}
do
    $Compiler -O3 -Wall -Werror -floop-parallelize-all -ftree-parallelize-loops=$loop ../lab1.c -o lab1-par-$loop -lm
    if [ $? -eq 0 ];then
        echo "Compile lab1-par-${loop} success!!!"
        programs=("${programs[@]}" lab1-par-${loop})
    fi
done

echo "programs: ${programs[@]}"

# run test
step=$((($N2-$N1)/10))
echo "N1=$N1, N2=$N2, step=$step"

if [ -d "./log" ]; then #if log exist, then remove it
    rm -rf log
fi
mkdir log


for program in ${programs[@]}
do
    echo "$program:"
    echo "$program:" >> log/total.log
    for i in $(seq 11)
    do
        N=$(($N1+$step*($i-1)))
        echo "test $i: N=$N"
	echo -e "\ntest $i: N=$N" >> log/$program.log
	./$program $N >> log/$program.log
    done
    cat log/$program.log | grep "Milliseconds passed" >> log/total.log
done


