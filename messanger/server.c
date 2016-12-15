#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

#define BLOCK 5

typedef struct request
{
    int type;
    char message[1024];
    char receiver[64];
}request;

typedef struct user
{
    char nick[64];
    struct sockaddr_in address;
}user;

int main(int argc, char* argv[])
{
    int sockfd;
    int clilen, n;
    int fail, ok;

    request req;

    //Пользователи
    user *users;
    int max = 0;
    int current = 0;
    int i, to, from;

    struct sockaddr_in servaddr, cliaddr;   

    //Инициаллизируем сокет
    if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror(NULL);
        exit(1);
    } 

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(51000);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
    {
        perror(NULL);
        close(sockfd);
        exit(1);
    }
    
    //Инициаллизируем динамическую память для работы с пользователями
    max += BLOCK;
    users = (user*)malloc(max * sizeof(user));

    //Основной цикл работы сервера
    while(1)
    {
        clilen = sizeof(cliaddr);
        if ((n = recvfrom(sockfd, &req, sizeof(req), 0,
                (struct sockaddr *) &cliaddr, &clilen)) < 0)
        {
            perror(NULL);
            close(sockfd);
            exit(1);
        }
	
        req.type = ntohs(req.type);
	
        //Выполняем различные просьбы пользователей
        if (req.type == 1)
        {
            //Регистрируем
            fail = 0;
            for (i = 0; i < current; i++)
            {
                if (!strcmp(req.message, users[i].nick))
                {
                    req.type = htons(3);
                    strcpy(req.message, "Such nick already exists\n");
                    if (sendto(sockfd, &req, sizeof(req), 0,
                            (struct sockaddr *) &cliaddr, clilen) < 0)
                    {
                        perror(NULL);
                        close(sockfd);
                        exit(1);
                    }
                    fail = 1;
                    break;
                }
            }

            if (!strcmp(req.message, "All"))
            {
                req.type = htons(3);
                strcpy(req.message, "Nick \"All\" is used to send messages to all recipients\n");
                if (sendto(sockfd, &req, sizeof(req), 0,
                    (struct sockaddr *) &cliaddr, clilen) < 0)
                {
                    perror(NULL);
                    close(sockfd);
                    exit(1);
                }
                fail = 1;
            }
            if (!fail)
            {
                //Добавляем памяти в случае необходимости
                if (max == current)
                {
                    max += BLOCK;
                    users = (user*)realloc(users, max * sizeof(user));
                }
                strcpy(users[current].nick, req.message);
                memcpy(&(users[current].address), &cliaddr, sizeof(cliaddr));
                current++;

                //Возвращаем подтверждение что все хорошо
                req.type = htons(2);
                if (sendto(sockfd, &req, sizeof(req), 0,
                    (struct sockaddr *) &cliaddr, clilen) < 0)
                {
                    perror(NULL);
                    close(sockfd);
                    exit(1);
                }
            }
        }

        if (req.type == 0)
        {
            //Отправка сообщений
            ok = 0;
            if (!strcmp(req.receiver, ""))
                ok = 1;
            else
            {
                for (i = 0; i < current; i++)
                {
                    //Проверяем существование ника адресата
                    if (!strcmp(req.receiver, users[i].nick))
                    {
                        printf("Ok\n");
                        to = i;
                        ok = 1;
                        break;
                    }
                }
            }

            if (!ok)
            {
                req.type = htons(3);
                strcpy(req.message, "Server can't find user with such nick\n");
                if (sendto(sockfd, &req, sizeof(req), 0,
                    (struct sockaddr *) &cliaddr, clilen) < 0)
                {
                    perror(NULL);
                    close(sockfd);
                    exit(1);
                }
                fail = 1;
            }

            if (!fail)
            {
                //Ищем имя того, кто отправил
                for (i = 0; i < current; i++)
                {
                    if ((users[i].address.sin_port == cliaddr.sin_port) &&
                        (users[i].address.sin_addr.s_addr == cliaddr.sin_addr.s_addr))
                    {
                        if (!strcmp(req.receiver, ""))
                            printf("Message from %s to all users:\n", users[i].nick);
                        else
                            printf("Message from %s to %s:\n",
                               users[i].nick, req.receiver);
                        from = i;
                        break;
                    }
                }
                puts(req.message);
                //Отправляем сообщение адресату
                req.type = htons(0);
                if (!strcmp(req.receiver, ""))
                {
                    strcpy(req.receiver, users[from].nick);
                    //Отправляем всем пользователям кроме отправителя
                    for (i = 0; i < current; i++)
                    {
                        if (i != from)
                        {
                            if (sendto(sockfd, &req, sizeof(req), 0,
                                       (struct sockaddr *) &users[i].address, clilen) < 0)
                            {
                                perror(NULL);
                                close(sockfd);
                                exit(1);
                            }
                        }
                    }
                }
                else
                {
                    //Отправляем конкретному пользователю
                    strcpy(req.receiver, users[from].nick);
                    if (sendto(sockfd, &req, sizeof(req), 0,
                        (struct sockaddr *) &users[to].address, clilen) < 0)
                    {
                        perror(NULL);
                        close(sockfd);
                        exit(1);
                    }
                }
                //Отправляем адресанту подтверждение об отправке
                req.type = htons(2);
                if (sendto(sockfd, &req, sizeof(req), 0,
                    (struct sockaddr *) &cliaddr, clilen) < 0)
                {
                    perror(NULL);
                    close(sockfd);
                    exit(1);
                }
            }
        }
    }

    return 0;
}