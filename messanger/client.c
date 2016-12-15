#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

//type = 0 - send message to receiver
//type = 1 - registration
//type = 2 - answer from server - OK
//type = 3 - answer from server - BAD

typedef struct request
{
    int type;
    char message[1024];
    char receiver[64];
}request;

int main(int argc, char* argv[])
{
    int sockfd;
    int len;
    
    request req;
    char nick[64];

    struct sockaddr_in servaddr, cliaddr;
    char IP[32];

    int pid;
    
    //Считываем IP адрес сервера из аргументов командной строки
    strcpy(IP, argv[1]);

    //инициализируем сокет
    if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
        {
            perror(NULL);
            exit(1);
        }

        bzero(&cliaddr, sizeof(cliaddr));
        cliaddr.sin_family = AF_INET;
        cliaddr.sin_port = htons(0);
        cliaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        if (bind(sockfd, (struct sockaddr *) &cliaddr, sizeof(cliaddr)) < 0)
        {
            perror(NULL);
            close(sockfd);
            exit(1);
        }

        //подготовливаем адрес сервера
        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(51000);
        if (inet_aton(IP, &servaddr.sin_addr) == 0)
        {
            printf("Invalid IP address\n");
            close(sockfd);
            exit(1);
        }

        //Регистрируемся
        do
        {
            printf("Your nick => ");
            
            fgets(nick, 63, stdin);
            nick[strlen(nick)-1] = '\0';

            req.type = htons(1);
            strcpy(req.message, nick);
    
            if (sendto(sockfd, &req, sizeof(req),
                       0, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
            {
                perror(NULL);
                close(sockfd);
                exit(1);
            }

            //Получаем подтверждение с сервера
            if ((len = recvfrom(sockfd, &req, sizeof(req), 0,
                    (struct sockaddr *) NULL, NULL)) < 0)
            {
                perror(NULL);
                close(sockfd);
                exit(1);
            }
    
            req.type = ntohs(req.type);
            if (req.type == 2)
                printf("Registration completed successfully\nYour nick: %s\n",
                       req.message);
            if (req.type == 3)
                printf("Registration failed.\nError: %s", req.message);
            } while(req.type != 2);

        pid = fork();
    
        if (pid > 0)
        {
            //Родитель занимается отправкой сообщений
            while(1)
            {
                printf("Addressee's nick => ");
                fgets(req.receiver, 63, stdin);
                req.receiver[strlen(req.receiver)-1] = '\0';
                printf("Message: ");
                fgets(req.message, 1023, stdin);
                req.message[strlen(req.message)-1] = '\0';
	
                req.type = htons(0);
    
                //Отправляем сообщение на сервер
                if (sendto(sockfd, &req, sizeof(req),
                        0, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
                {
                    perror(NULL);
                    close(sockfd);
                    exit(1);
                }
                sleep(1);
            }
        }

        if (pid == 0)
        {
            //Ребенок занимается приемом сообщений
            while(1)
            {
            //Принимаем сообщения
            if ((len = recvfrom(sockfd, &req, sizeof(req), 0,
                    (struct sockaddr *) NULL, NULL)) > 0)
            {
                req.type = ntohs(req.type);
                if (req.type == 0)
                {
                    printf("\nMessage from %s:\n", req.receiver);
                    puts(req.message);
                }
		
                if (req.type == 2)
                    printf("Message sent\n");
                if (req.type == 3)
                    printf("\nError: %s", req.message);
            }
        }
    }
    
    close(sockfd);

    return 0;
}