
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
    
    int msqid;
    int number1;
    int number2;
    int maxlen;
    number1 = rand() % 20;
    number2 = rand() % 20;
    printf ("number1 = %d", number1);
    printf ("number2 = %d", number2);
    char pathname[] = "client-server.c";
    
  
    key_t key;
    
    int i, len;
    
    
    struct mymsgbuf
    {
        long mtype;
        struct {
            int numbers[2];
            int pid;
        }info;
    } mybuf;
    
  
    if ((key = ftok(pathname, 0)) < 0)
    {
        printf("Can\'t generate key\n");
        exit(-1);
    }
   
    
    if((msqid = msgget(key, 0666 | IPC_CREAT)) < 0)
    {
        printf("Can\'t get msqid\n");
        exit(-1);
    }
    
  
    mybuf.mtype = 1;
    mybuf.info.numbers[0] = number1;
    mybuf.info.numbers[1] = number2;
    mybuf.info.pid = getpid();
    len = sizeof(mybuf.info);
        
    if (msgsnd(msqid, (struct msgbuf *) &mybuf, len, 0) < 0)
    {
        printf("Can\'t send message to queue\n");
        msgctl(msqid, IPC_RMID, (struct msqid_ds*)NULL);
        exit(-1);
    }
    
    maxlen = sizeof(mybuf.info);
    
    if ((len = msgrcv(msqid, (struct msgbuf *)&mybuf, maxlen, mybuf.info.pid, 0)) < 0)
    {
        printf("Can\'t receive message from queue\n");
        exit(-1);
    }
    printf ("answer = %d\n",mybuf.info.numbers[0]);
    return 0;
}