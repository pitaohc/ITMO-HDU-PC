#!/bin/bash
# author: pitaohc@qq.com
threads=(1 4 8 16)	# 多线程优化的线程数
N1=235		# 10ms N1
N2=1575		# 2000ms N2
Compiler=gcc	# 编译器

# 创建构建目录
if [ ! -d "./build_$Compiler" ]; then #if build doesn't exist, then create it
	mkdir build_$Compiler
fi
cd build_$Compiler

# 编译目标 compile all targets
$Compiler -O3 -Wall -Werror -o lab3-seq ../lab3.c -lm # 编译无并行目标
if [ $? -eq 0 ];then
    echo 'Compile seq success!!!'
    programs=("${programs[@]}" lab3-seq) # 添加程序文件名称到程序数组
fi

# 编译并行目标 compile parallel target
for loop in ${threads[@]}
do
    $Compiler -O3 -Wall -Werror -floop-parallelize-all -ftree-parallelize-loops=$loop ../lab3.c -o lab3-par-$loop -lm -fopenmp
    if [ $? -eq 0 ];then
        echo "Compile lab3-par-${loop} success!!!"
        programs=("${programs[@]}" lab3-par-${loop})
    fi
done
threads=(1 "${threads[@]}")
echo "threads: ${threads[@]}"
echo "programs: ${programs[@]}"

# 运行测试 run test
step=$((($N2-$N1)/10)) # 步长
echo "N1=$N1, N2=$N2, step=$step"

# 创建日志文目录 create log directory
if [ -d "./log" ]; then #if log exist, then remove it
    rm -rf log
fi
mkdir log

# 运行程序 execute programs
for i in $(seq 5)
do
    program=${programs[i-1]}
    thread=${threads[i-1]}
    echo "$program: $thread"
    echo "$program:" >> log/total.log # 输出程序名称到总日志
    for j in $(seq 11) # 每个程序测试11次 run program by different N
    do
        N=$(($N1+$step*($j-1)))
        echo "test $j: N=$N"
        echo -e "\ntest $j: N=$N M=$thread" >> log/$program.log
        ./$program $N ${thread-1} >> log/$program.log # input N to program as argument 将N作为参数输入到程序中
    done
    cat log/$program.log | grep "Milliseconds passed" >> log/total.log # 截取程序输出中的时间关键字Milliseconds passed并记录到总日志中
done
