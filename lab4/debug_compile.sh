cd build
echo "lab4-seq"
gcc -O3 -Wall -Werror ../lab4.c -o lab4-seq -lm
#./lab4-seq 10000 8
echo "lab4-openmp"
gcc -O3 -Wall -Werror ../lab4.c -o lab4-openmp -lm -fopenmp
./lab4-openmp 5000 8 >> ./run.log
#gcc -O3 -Wall -Werror -floop-parallelize-all -ftree-parallelize-loops=4 ../lab4.c -o lab4-auto -lm
#gcc -O3 -Wall -Werror -floop-parallelize-all -ftree-parallelize-loops=4 ../lab4.c -o lab4-com -lm -fopenmp

#echo "lab4-auto"
#./lab4-auto 100 8
#echo "lab4-com"
#./lab4-com 100 8