//
//  main.c
//  fork.c
//
//  Created by Мария on 29.09.16.
//  Copyright (c) 2016 Мария. All rights reserved.
//

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


int main(int argc, const char * argv[]) {
    int i;
    int N;
    scanf("%d", &N);
    pid_t child;
    for (i = 0; i < N; ++i)
    {
        pid_t pid = fork();
        if (pid == 0)
            printf("Hello: child pid - %d\nparent pid - %d\n", getpid(), getppid());
        else
        {
            child = pid;
            break;
        };
    }
    sleep(N - i);
    if (i!=N)
        printf("Child with pid %d is dead\n", child);
    
    return 0;
}
