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
$Compiler -O3 -Wall -Werror -o lab1-seq ../lab1.c -lm # 编译无并行目标
if [ $? -eq 0 ];then
    echo 'Compile seq success!!!'
    programs=("${programs[@]}" lab1-seq) # 添加程序文件名称到程序数组
fi

# 编译并行目标 compile parallel target
for loop in ${threads[@]}
do
    $Compiler -O3 -Wall -Werror -floop-parallelize-all -ftree-parallelize-loops=$loop ../lab1.c -o lab1-par-$loop -lm 
    if [ $? -eq 0 ];then
        echo "Compile lab1-par-${loop} success!!!"
        programs=("${programs[@]}" lab1-par-${loop})
    fi
done

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
for program in ${programs[@]}
do
    echo "$program:"
    echo "$program:" >> log/total.log # 输出程序名称到总日志
    for i in $(seq 11) # 每个程序测试11次 run program by different N
    do
        N=$(($N1+$step*($i-1)))
        echo "test $i: N=$N"
        echo -e "\ntest $i: N=$N" >> log/$program.log
        ./$program $N >> log/$program.log # input N to program as argument 将N作为参数输入到程序中
    done
    cat log/$program.log | grep "Milliseconds passed" >> log/total.log # 截取程序输出中的时间关键字Milliseconds passed并记录到总日志中
done
