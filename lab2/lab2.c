#include <stdlib.h>
#include <sys/time.h>
#include <stdio.h>
#include <math.h>
#include <fwBase.h>
#include <fwSignal.h>


#define A 24
#define SEED 34
#define DEFAULT_N 100
#define DEFAULT_M 8

double* fill_array(double *arr, size_t size, unsigned int min, unsigned int max);
double reduce(double *arr, size_t size);
void stupid_sort(double *arr, size_t size);
void print_array(double *arr, size_t size,char* message);

int main(int argc, char* argv[]) {
    struct timeval T1, T2; //record time
    long delta_ms=0; // 时间差值 T2-T1
    int N; // size of array
    if(argc > 1)
    {
        N = (size_t) atoi(argv[1]);  //get N from argument 读取参数
    }
    else
    {
        N = DEFAULT_N; //if not argument , set N by default
    }

    int M; //M is the number of threads
    if(argc > 2)
    {
        M = (size_t) atoi(argv[2]);  //get M from argument 读取参数
    }
    else
    {
        M = DEFAULT_M; //if not argument , set M by default
    }
    fwSetNumThreads(M);
    
//    double x, *m1 = (double*)malloc(sizeof(double) * N), *m2 = (double*)malloc(sizeof(double) * (N / 2));
    double x, m1[N], m2[N/2];
    printf("N=%d\n",N);
    gettimeofday(&T1, NULL);
    for (int i = 0; i < 50; ++i) {
        // Generate Stage.
        fill_array(m1, N, 0, A);
        fill_array(m2, N/2, A, 10*A);

        // Map Stage.
        fwsSqrt_64f	(m1, m1, N );
        fwsTanh_64f_A50	(m1, m1, N );
        double numerator[N];
        for (int j = 0; j < N; j++)
        {
            numerator[j] = 1.0;
        }
        fwsDiv_64f_A50 (numerator, m1, m1, N );
        // map2(m2, N/2);
        double pre[N/2];
        pre[0] = 0.0f;
        for (int k = 1; k < N/2; ++k) {
            pre[k] = m2[k - 1];
        }
        fwsAdd_64f (m2, pre, m2, N / 2);
        fwsSin_64f_A50 (m2, m2, N / 2 );
        fwsAbs_64f_I (m2, N / 2 );
        // Merge Stage.
        fwsPow_64f_A50 (m1, m2, m2, N / 2 );
        // Sort Stage.
        stupid_sort(m2, N/2);
        // Reduce Stage.
        x = reduce(m2, N/2);
        // Output X
        fprintf(stdout,"%d => X:%f\n",i+1,x);
    }

    gettimeofday(&T2, NULL);
    // Output delta time(ms)
    delta_ms = 1000 * (T2.tv_sec - T1.tv_sec) + (T2.tv_usec - T1.tv_usec) / 1000; 
    printf("N=%d. Milliseconds passed: %ld\n", N, delta_ms); /* T2 -T1 */
    // Release memory distributed by malloc.
    //free(m1);
    //free(m2);
    return 0;
}

/*
* 填充数组
* fill an array by random number in [min,max]
* @param arr 
* @param size: length of array
* @param min: mininum of random range
* @param max: maxinum of random range
*/
double* fill_array(double *arr, size_t size, unsigned int min, unsigned int max)
{
//    unsigned int seed = SEED;
    struct  timeval time;
    gettimeofday(&time,NULL);
    unsigned int seed = time.tv_usec+time.tv_sec; // use current time as random seed.
    for (int i = 0; i < size; i++)
        arr[i] = ((double) (rand_r(&seed)%(100*(max-min)))/100) + min;
    return arr;
}

/*
* 排序阶段
* sort stage: stupid sort
* @param arr
* @param size: length of array
*/
void stupid_sort(double *arr, size_t size) {
    int i = 0;
    double tmp;
    while (i < size - 1) {
        if (arr[i + 1] < arr[i]) {
            tmp = arr[i];
            arr[i] = arr[i + 1];
            arr[i + 1] = tmp;
            i = 0;
        } else
            i++;
    }
}

/*
* 累加阶段
* reduce stage: get the mininum number of array. if floor(arr[i]/min) is even, then the sum adds it's sine.
* @param arr
* @param size: length of array
*/
double reduce(double *arr, size_t size) {
    double res = 0, min;
    int i = 0;
    min = arr[0];
    // find the mininum number which not equal 0.
    for (i = 0; i < size; i++) {
        if (arr[i] < min && (int)arr[i] != 0) {
            min = arr[i];
        }
    }

    // reduce
    for (i = 0; i < size; i++) {
        if ((int)(arr[i] / min) % 2 == 0) {
            res += sin(arr[i]);
        }
    }
    return res;
}

/*
* 打印数组
* debug function: print the array
* @param arr
* @param size: length of array
* @param message: extra message
*/
void print_array(double *arr, size_t size,char* message) {
    printf("%s\n",message);
    for (size_t i = 0; i < size; ++i) {
        printf("%.4f ",arr[i]);
    }
    printf("\n");
}
