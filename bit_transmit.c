#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#define MAX 64

void send(int sign);
void receive(int sign);
int power(int n);

pid_t pid, ppid;
char message[] = "RT_CHAMPION!";
char record[MAX];

int bits;

int main()
{
    
    (void) signal(SIGUSR1, receive);
    (void) signal(SIGUSR2, receive);
    (void) signal(SIGINT, send);
    bits = 0;
    ppid = getpid();
    pid = fork();
    if (pid > 0)
    {
	send(0);
    }
    while(1);

    return 0;
}

void send(int sign)
{
    if (bits >= 8*(1+strlen(message))){
	exit(0);
    }
    if ((message[(bits/8)] & power(bits%8)))
	kill(pid, SIGUSR1);
    else kill(pid, SIGUSR2);
    bits++;
}

void receive(int sign)
{
    if (sign == SIGUSR1)
	record[bits/8] += (char)power(bits%8);
    bits++;
    kill(ppid, SIGINT);
    if (!(bits % 8) && (record[bits/8 - 1] == '\0'))
    {
	printf("%s\n", record);
	exit(0);
    }
}

int power(int n)
{
    int res = 1;
    while(n)
    {
	res *= 2;
	n--;
    }
    return res;
}