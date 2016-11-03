
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define N 2

int semid;
struct sembuf mybuf;

void sem(int operation)
{
    mybuf.sem_num = 0;
    mybuf.sem_op = operation;
    mybuf.sem_flg = 0;
    if (semop(semid, &mybuf, 1) < 0)
    {
        printf("Can't wait for condition");
        exit(-1);
    }
}

int main()
{
    
    int msqid;
    int answer;
    
    pid_t pid;
    
    
    char pathname[] = "client-server.c";
    
   
    key_t key;
    
   
    int len, maxlen;
    
    
    struct mymsgbuf
    {
        long mtype;
        struct {
            int numbers[2];
            int pid;
        }info;
    } mybuf;
    
    
    
    if((key = ftok(pathname, 0)) < 0)
    {
        printf("Can\'t generate key\n");
        exit(-1);
    }
   
    
    if((msqid = msgget(key, 0666 | IPC_CREAT)) < 0)
    {
        printf("Can\'t get msqid\n");
        exit(-1);
    }
    
    if ((semid = semget(key, 1, 0666 | IPC_CREAT)) < 0)
    {
        printf("Can't create semaphore\n");
        exit(-1);
    }
    
    sem(N);
    
    while (1)
    {
        maxlen = sizeof(mybuf.info);
        
        if ((len = msgrcv(msqid, (struct msgbuf *)&mybuf, maxlen, 1, 0)) < 0)
        {
            printf("Can\'t receive message from queue\n");
            exit(-1);
        }
        
        sem(-1);
        
        if ((pid = fork()) < 0)
        {
            printf("Can't create process\n");
            exit(-1);
        }
        else if (pid == 0)
        {
            answer = mybuf.info.numbers[0] * mybuf.info.numbers[1];
            sleep(7);
            mybuf.mtype = mybuf.info.pid;
            mybuf.info.numbers[0] = answer;
            len = sizeof(mybuf.info);
            
            if (msgsnd(msqid, (struct msgbuf *) &mybuf, len, 0) < 0)
            {
                printf("Can\'t send message to queue\n");
                msgctl(msqid, IPC_RMID, (struct msqid_ds*)NULL);
                exit(-1);
            }
            
            break;
        }

    }
    
    sem(1);

    return 0;
}