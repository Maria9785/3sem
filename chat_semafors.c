#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int semid;
struct sembuf mybuf;

int main() {
    int fdparent[2];
    int writen;
    int size = 0;
    int num1 = 2, num2 = 2, sum;
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
    
    
    
    
    
    pipe(fdparent);
    pid_t pid = fork();
    if (pid == 0) {
        sem(-1, 1);
        read(fdparent[0], &num1, sizeof(int));
        read(fdparent[0], &num2, sizeof(int));
        printf("%d\n %d\n", num1, num2);
        sum = num1 + num2;
        write(fdp[1], &sum, sizeof(int));
        sem(1,0);
        
    }
    else
        write(fdparent[1], &num1, sizeof(int));
    write(fdparent[1], &num2, sizeof(int));
    sem(-1, 0);
    read(fdparent[0], &sum, sizeof(int));
    sem(1,1);
    printf("%d\n", sum);
    
}

return 0;
}

void sem (int a,int b)
{
    
    mybuf.sem_op = a;
    mybuf.sem_flg = 0;
    mybuf.sem_num = b;
    
    if (semop(semid , &mybuf , 1) < 0) {
        
        printf("Can`t wait for condition\n");
        exit(-1);
        
    }