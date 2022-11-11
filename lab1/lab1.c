//
// Created by pc on 11/1/22.
//

#include <stdlib.h>
#include <sys/time.h>
#include <stdio.h>
#include <math.h>

#define A 24
#define SEED 34
#define DEFAULT_N 100

double* fill_array(double *arr, size_t size, unsigned int min, unsigned int max);
void map1(double *arr, size_t size);
void map2(double *arr, size_t size);
double reduce(double *arr, size_t size);
void merge(double *arr1, double *arr2, size_t size2);
void stupid_sort(double *arr, size_t size);
void print_array(double *arr, size_t size,char* message);
int main(int argc, char* argv[]) {
    struct timeval T1, T2;
    long delta_ms=0;
    int N;
    if(argc > 1)
        N = (size_t) atoi(argv[1]);  //读取参数
    else
        N = DEFAULT_N;
    double x, *m1 = (double*)malloc(sizeof(double) * N), *m2 = (double*)malloc(sizeof(double) * (N / 2));

    gettimeofday(&T1, NULL);
    for (int i = 0; i < 50; ++i) {
        fill_array(m1, N, 0, A);
        fill_array(m2, N/2, A, 10*A);

        map1(m1, N);
        map2( m2, N/2);

        merge(m1, m2, N/2);
        stupid_sort(m2, N/2);
        x = reduce(m2, N/2);

        printf("%d => X:%f\n",i+1,x);
    }

    gettimeofday(&T2, NULL); /* запомнить текущее время T2 */
    delta_ms = 1000 * (T2.tv_sec - T1.tv_sec) + (T2.tv_usec - T1.tv_usec) / 1000;

    printf("\nN=%d. Milliseconds passed: %ld\n", N, delta_ms); /* T2 -T1 */
    free(m1);
    free(m2);
    return 0;
}

/*
* 填充数组
*/
double* fill_array(double *arr, size_t size, unsigned int min, unsigned int max)
{
    unsigned int seed = SEED;
    for (int i = 0; i < size; i++)
        arr[i] = ((double) (rand_r(&seed)%(100*(max-min)))/100) + min;
    return arr;
}

/*
* 使用方法1映射数组
*/
void map1(double *arr, size_t size) {
    for (int i = 0; i < size; i++) {
        arr[i]= sqrt(1.0/ tanh(arr[i]));
//        arr[i]= sqrt(1.0/ tanh(arr[i]));
    }
}

/*
* 使用方法2映射数组
*/
void map2(double *arr,size_t size)
{
    double prev = 0.0f;
    for (int i = 0; i < size; i++) {
        prev = arr[i]= fabs(sin(arr[i]+prev));
    }
}

/*
* 合并阶段
*/
void merge(double *arr1, double *arr2, size_t size2) {
    for (int i = 0; i < size2; i++) {
        arr2[i] = pow(arr1[i], arr2[i]);
    }
}

/*
* 排序阶段
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
//    return 0;
}

/*
* 累加阶段
*/
double reduce(double *arr, size_t size) {
    double res = 0, min;
    int i = 0;
    min = arr[0];

    for (i = 0; i < size; i++) {
        if (arr[i] < min && arr[i] != 0) {
            min = arr[i];
        }
    }

    for (i = 0; i < size; i++) {
        if ((int)(arr[i] / min) % 2 == 0) {
            res += sin(arr[i]);
        }
    }
    return res;
}

/*
* 打印数组
*/
void print_array(double *arr, size_t size,char* message) {
    printf("%s\n",message);
    for (size_t i = 0; i < size; ++i) {
        printf("%.4f ",arr[i]);
    }
    printf("\n");
}
