gcc -O3 -Wall -Werror -o lab1-seq lab1.c -lm
gcc -O3 -Wall -Werror -floop-parallelize-all -ftree-parallelize-loops=8 lab1.c -o lab1-par-8 -lm