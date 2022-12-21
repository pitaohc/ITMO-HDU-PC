cd build
gcc -O3 -Wall -Werror ../lab4.c -o lab4-seq -lm
gcc -O3 -Wall -Werror ../lab4.c -o lab4-openmp -lm -fopenmp
#gcc -O3 -Wall -Werror -floop-parallelize-all -ftree-parallelize-loops=4 ../lab4.c -o lab4-auto -lm
#gcc -O3 -Wall -Werror -floop-parallelize-all -ftree-parallelize-loops=4 ../lab4.c -o lab4-com -lm -fopenmp
echo "lab4-seq"
./lab4-seq 100 8
echo "lab4-openmp"
./lab4-openmp 100 8
#echo "lab4-auto"
#./lab4-auto 100 8
#echo "lab4-com"
#./lab4-com 100 8