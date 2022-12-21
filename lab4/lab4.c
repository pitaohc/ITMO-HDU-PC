// Created by pitaohc on 11/1/22.
//

#include <stdlib.h>
#include <sys/time.h>
#include <stdio.h>
#include <math.h>

//#ifndef _OPENMP
//#define _OPENMP
//#endif

#ifdef _OPENMP
#include<omp.h>
#endif
#define A 24
#define SEED 34
#define DEFAULT_N 100
#define DEFAULT_M 4
#define ROUND 1

#define NONE         "\033[m"
#define RED          "\033[0;32;31m"
#define LIGHT_RED    "\033[1;31m"
#define GREEN        "\033[0;32;32m"
#define LIGHT_GREEN  "\033[1;32m"
#define BLUE         "\033[0;32;34m"
#define LIGHT_BLUE   "\033[1;34m"
#define DARY_GRAY    "\033[1;30m"
#define CYAN         "\033[0;36m"
#define LIGHT_CYAN   "\033[1;36m"
#define PURPLE       "\033[0;35m"
#define LIGHT_PURPLE "\033[1;35m"
#define BROWN        "\033[0;33m"
#define YELLOW       "\033[1;33m"
#define LIGHT_GRAY   "\033[0;37m"
#define WHITE        "\033[1;37m"
double* fill_array(double *arr, size_t size, unsigned int min, unsigned int max);
void map1(double *arr, size_t size);
void map2(double *arr, size_t size);
double reduce(double *arr, size_t size);
void merge(double *arr1, double *arr2, size_t size2);
void stupid_sort(double *arr, size_t size);
double* merge_sorted_arr(double *arr1, size_t size1,double *arr2, size_t size2);
void print_array(double *arr, size_t size,char* message);
double get_time();
void monitor();
int N,M; // size of array, number of threads

int main(int argc, char* argv[]) {
#ifdef _OPENMP
#pragma omp parallel num_threads(1)
    {
        monitor();
    }
#endif
    double T1, T2; //record time
    double delta_ms=0; // 时间差值 T2-T1

    if(argc > 1)
    {
        N = (size_t) atoi(argv[1]);  //get N from argument 读取参数
    }
    else
    {
        N = DEFAULT_N; //if not argument , set N by default
    }

    if (argc > 2)
    {
        M = (size_t)atoi(argv[2]);
    }
    else 
    {
        M = DEFAULT_M;
    }
    // 设置线程数
#ifdef _OPENMP
    printf("number of threads: %d\n",M);
    omp_set_num_threads(M);
#endif
    double x, *m1 = (double*)malloc(sizeof(double) * N), *m2 = (double*)malloc(sizeof(double) * (N / 2));
    printf("N=%d\n",N);
    T1 = get_time();
    for (int i = 0; i < ROUND; ++i) {
        // Generate Stage.
        fill_array(m1, N, 0, A);
        fill_array(m2, N/2, A, 10*A);
        // Map Stage.
        map1(m1, N);
        map2( m2, N/2);
        // Merge Stage.
        merge(m1, m2, N/2);
        // Sort Stage.
        double sort_begin,sort_end;
        sort_begin = get_time();
#ifdef _OPENMP
        //TODO:修改合并阶段，能够生成k个线程对数组排序
#pragma omp parallel sections
        {
#pragma omp section
            {
                stupid_sort(m2, N/4);
            }
#pragma omp section
            {
                stupid_sort(m2+N/4, N/4);
            }
        }
        double* m2_temp = merge_sorted_arr(m2,N/4,m2+N/4,N/4);
        free(m2); //释放原先的内存位置 free original memory to avoid memory leaking
        m2 = m2_temp;
#else
        stupid_sort(m2, N/2);
#endif
        sort_end = get_time();
        printf("sort stage time: %.4fms\n",sort_end-sort_begin);

        // Reduce Stage.
        x = reduce(m2, N/2);
        // Output X
        fprintf(stdout,"%d => X:%f\n",i+1,x);
    }
    T2 = get_time();
    // Output delta time(ms)
    delta_ms = T2 - T1;
    printf("N=%d. Milliseconds passed: %.4fms\n", N, delta_ms); /* T2 -T1 */
    // Release memory distributed by malloc.
    free(m1);
    free(m2);
    return 0;
}

/**
* 填充数组
* fill an array by random number in [min,max]
* @param arr 
* @param size: length of array
* @param min: mininum of random range
* @param max: maxinum of random range
*/
double* fill_array(double *arr, size_t size, unsigned int min, unsigned int max)
{
    unsigned int seed = SEED;
    double begin = get_time();
    int i;
#ifdef _OPENMP
#pragma omp parallel for private(i) shared(arr)  schedule(guided,M)
#endif
    for (i = 0; i < size; i++)
    {
        arr[i] = ((double) (rand_r(&seed)%(100*(max-min)))/100) + min;
    }
    double end = get_time();
    printf("fill stage time: %.4fms\n",end-begin);
    return arr;
}

/**
* 使用方法1映射数组
* map elements in array by hyperbolic cotangent of number’s root
* @param arr
* @param size: length of array
*/
void map1(double *arr, size_t size) {
    int i;
    double begin = get_time();
#ifdef _OPENMP
#pragma omp parallel for private(i) shared(arr)  schedule(guided,M)
#endif
    for (i = 0; i < size; i++) {
        arr[i] = 1.0 / tanh(sqrt(arr[i]));
    }
    double end = get_time();
    printf("map stage 1 time: %.4fms\n",end-begin);

}

/**
* 使用方法2映射数组
* map elements in array by sine modulus
* @param arr
* @param size: length of array
*/
void map2(double *arr,size_t size)
{
    int i;
    double begin = get_time();
    double* copyed_arr = (double*)malloc(sizeof(double) * size);
#ifdef _OPENMP
#pragma omp parallel for private(i) shared(arr,copyed_arr)  schedule(guided,M)
#endif
    for(i=0;i<size;++i)
    {
        copyed_arr[i] = arr[i];
    }

    arr[0]= fabs(sin(arr[0]));
#ifdef _OPENMP
#pragma omp parallel for private(i) shared(arr,copyed_arr)  schedule(guided,M)
#endif
    for (i = 1; i < size; i++)
    {

        arr[i]= fabs(sin(arr[i]+copyed_arr[i-1]));
    }
    double end = get_time();
    printf("map stage 2 time: %.4fms\n",end-begin);

}

/**
* 合并阶段
* merge stage: Raising to a power
* @param arr1: array 1
* @param arr2: array 2
* @param size2: length of array 2
*/
void merge(double *arr1, double *arr2, size_t size2)
{
    int i;
#ifdef _OPENMP
#pragma omp parallel for private(i) shared(arr1,arr2)  schedule(guided,M)
#endif
    for (i = 0; i < size2; i++)
    {
        arr2[i] = pow(arr1[i], arr2[i]);
    }
}

/**
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

/**
 * 累加阶段
 * reduce stage: get the mininum number of array. if floor(arr[i]/min) is even, then the sum adds it's sine.
 * TODO:修改累加阶段，支持并行化
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

/**
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

/**
 * 获得当前时间，单位ms
 * */
double get_time()
{
    double result=0.0f;
#ifdef _OPENMP
    result =  omp_get_wtime() * 1000;
#else
    struct timeval T;
    gettimeofday(&T, NULL);
    result = 1000 * T.tv_sec + T.tv_usec / 1000;
//    result =  delta / 1000;
#endif
    return result;
}
/**
 * 合并两个有序数组
 * TODO:修改合并函数，能够合并k个有序数组
 * */
double* merge_sorted_arr(double *arr1, size_t size1,double *arr2, size_t size2) {
    int p1=0,p2=0,p_r=0;
    double* result_arr = (double*) malloc((size1+size2)*sizeof(double));
    while(p1<size1 && p2 < size2)
    {
        result_arr[p_r++]=(arr1[p1]<arr2[p2])?arr1[p1++]:arr2[p2++];
    }
    while(p1<size1) //补上arr1剩余元素
    {
        result_arr[p_r++]=arr1[p1++];
    }
    while(p2<size2) //补上arr2剩余元素
    {
        result_arr[p_r++]=arr2[p2++];
    }
    return result_arr;
}
/**
 * 监视函数
 * TODO:完成监视函数，展示百分比和进度条
 * */
void monitor() {
//    printf( RED "current function is %s " RED " file line is %d\n" NONE,
//            __FUNCTION__, __LINE__ );
    printf(RED "Monitor: Start!!!\n"NONE);
}
