#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#define Number_Of_Threads 2
#define Numbers_Of_Elements 100000000


double segment_sum[Number_Of_Threads];
double numbers[Numbers_Of_Elements];
double segment_squared_sum[Numbers_Of_Elements];



struct Segment {
    int begin;
    int end;
    int index;
};

void* my_thread(void* arg) {
    struct Segment segment = *((struct Segment*)arg);
    
    int i;
    for (i = segment.begin; i < segment.end; i++)
    {
        segment_sum[segment.index] += numbers[i];
        segment_squared_sum[segment.index] += numbers[i] * numbers[i];
        
    }
    return NULL;
}


int main()
{
    pthread_t thread_id[Number_Of_Threads];
    int result[Number_Of_Threads];
    int i;
    int length = Numbers_Of_Elements / Number_Of_Threads;
    struct Segment segments[Number_Of_Threads];
    for (i = 0; i < Number_Of_Threads; i++)
    {
        segments[i].index = i;
        segments[i].begin = i * length;
        segments[i].end = segments[i].begin + length;
    }
    for (i = 0; i < Numbers_Of_Elements; i++)
        numbers[i] = rand() % 3 + 1;
    
    clock_t begin = clock();
    
    for (i = 0; i < Number_Of_Threads; i++)
    {
        result[i] = pthread_create(&(thread_id[i]) ,
                                (pthread_attr_t *)NULL ,
                                my_thread ,
                                &(segments[i]));
    
        if (result[i]) {
        printf("Can`t create thread, returned value = %d\n" , result[i]);
        exit(-1);
        }
    }
    
    for (i = 0; i < Number_Of_Threads; i++)
            pthread_join(thread_id[i] , (void **) NULL);
    clock_t end = clock();
    int sum = 0;
    int squared_sum = 0;
    for (i = 0; i < Number_Of_Threads; i++)
    {
        sum += segment_sum[i];
        squared_sum += segment_squared_sum[i];
    }
    float average = (float)sum / Numbers_Of_Elements;
    float dispersion = (float)squared_sum / Numbers_Of_Elements - average * average;
    printf (" sum= %d\n average= %f\n dispersion= %f\n", sum, average, dispersion);
    double time = (double)(end - begin);
    printf("time spent = %lf\n", time);
    return 0;
}
