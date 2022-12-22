#!/bin/bash
# author: pitaohc@qq.com
threads=(1 2 4 8 16 32)	# 多线程优化的线程数
NX=50
N1=235		# 10ms N1
N2=1575		# 2000ms N2
Nmin=${NX}
Nmax=${N2}
Compiler=gcc	# 编译器

# 创建构建目录
if [ ! -d "./build_$Compiler" ]; then #if build doesn't exist, then create it
	mkdir build_$Compiler
fi
cd build_$Compiler

# 编译目标 compile all targets
$Compiler -O3 -Wall -Werror -o lab4-seq ../lab4.c -lm # 编译无并行目标
if [ $? -eq 0 ];then
    echo 'Compile seq success!!!'
    programs=("${programs[@]}" lab4-seq) # 添加程序文件名称到程序数组
fi

# 编译并行目标 compile parallel target
$Compiler -O3 -Wall -Werror -o lab4-openmp ../lab4.c -lm -fopenmp # 编译并行目标
if [ $? -eq 0 ];then
    echo 'Compile openmp success!!!'
    programs=("${programs[@]}" lab4-openmp) # 添加程序文件名称到程序数组
fi


echo "threads: ${threads[@]}"
echo "programs: ${programs[@]}"

# 运行测试 run test
step=$((($Nmax-Nmin)/10)) # 步长
echo "Nmin=$Nmin, Nmax=$Nmax, step=$step"

# 创建日志文目录 create log directory
if [ -d "./log" ]; then #if log exist, then remove it
    rm -rf log
fi
mkdir log

# 运行程序 execute programs
program=${programs[0]}
thread=${threads[0]}
echo "$program: $thread"
echo "$program:" >> log/total.log # 输出程序名称到总日志
for j in $(seq 11) # 每个程序测试11次 run program by different N
do
    N=$(($Nmin+$step*($j-1)))
    echo "test $j: N=$N"
    echo -e "\ntest $j: N=$N M=$thread" >> log/$program.log
    ./$program $N ${thread-1} >> log/$program.log # input N to program as argument 将N作为参数输入到程序中
done
cat log/$program.log | grep "Milliseconds passed" >> log/total.log # 截取程序输出中的时间关键字Milliseconds passed并记录到总日志中

program=${programs[1]}
for thread in ${threads[@]}
do

  echo "$program: $thread"
  echo "$program: $thread" >> log/total.log # 输出程序名称到总日志
  logfile=${program}-${thread}.log
  for j in $(seq 11) # 每个程序测试11次 run program by different N
  do
      N=$(($Nmin+$step*($j-1)))
      echo "test $j: N=$N"
      echo -e "\ntest $j: N=$N M=$thread" >> log/${logfile}
      ./${program} ${N} ${thread} >> log/${logfile} # input N to program as argument 将N作为参数输入到程序中
  done
  cat log/${logfile} | grep "Milliseconds passed" >> log/total.log # 截取程序输出中的时间关键字Milliseconds passed并记录到总日志中

done