#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include <stdlib.h>
#include <pthread.h>


#define Number_Of_Threads 2

int semid;
struct sembuf mybuf;

void sem(int a, int b) {
    mybuf.sem_op = a;
    mybuf.sem_flg = 0;
    mybuf.sem_num = b;
    if (semop(semid, &mybuf, 1) < 0)
        printf("plok\n");
}

void* my_thread(void* dummy) {

    pthread_t mythid;
    mythid = pthread_self();
    while(1)
    {
        printf ("--try to lock sem 2\n");
        sem(-1,1);
        printf ("sem 2 is locked\n--try to lock sem1\n");
        sem(-1,0);
        printf("sem 1 is locked");
        sem(1,0);
        sem(1,1);
    }
    return NULL;
}

int main()
{
    pthread_t thread_id;
    int result;
    
    const char pathname[] = "s08e01a.c";
    
    
    
    key_t key;
    
    
    if ( (key = ftok(pathname , 0)) < 0 ) {
        printf("Can`t generate key\n");
        exit(-1);
    }
    
    
    if ((semid = semget(key , 2 , 0666 | IPC_CREAT)) < 0) {
        printf("Can`t get semid\n");
        exit(-1);
    }
    sem(1, 1);
    sem(1, 0);
    


        result = pthread_create(&thread_id ,
                                   (pthread_attr_t *)NULL ,
                                   my_thread, NULL);

        if (result != 0)
        {
            printf("Can`t create thread, returned value = %d\n" , result);
            exit(-1);
        }
        pthread_join(thread_id , (void **) NULL);


    while (1)
    {
        printf("--try to lock sem 1");
        sem(-1,0);
        printf ("--sem 1 is locked\n --try to lock sem 2");
        sem(-1,1);
        printf("--sem 2 is locked");
        sem(1,0);
        sem(1,1);
        
    }
  

        


    return 0;
}