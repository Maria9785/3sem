#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <string.h>

typedef struct time
{
    int wash;
    int dry;
} time;

//Операции над семафорами
void sem_operation(int num, int operation);

//Для работы с семафорами
int semid;
struct sembuf bufer;

int main(int argc, char *argv[], char *env[])
{
    char sub_string[] = "TABLE_LIMIT=";
    char *part_str;
    int i = 0;
    int N;

    //Работа с файлами 
    FILE *file;
    time *times;
    int num_types;
    int type;

    //Для работы с разделяемой памятью
    int *shared;
    int shmid;
    char path_name[] = "washer.c";
    key_t key;

    int plate;

/*...............................................*/

    //Находим N из переменных окружения 
    while (env[i])
    {
        part_str = strstr(env[i], sub_string);
        if (part_str) break;
        i++;
    }

    if (!part_str)
    {
        printf("Can't find variable in the enviroment\n");
        exit(-1);
    }

    part_str +=strlen(sub_string);
    N = atoi(part_str);

    //Чтение из файлов
    //Типы
    file = fopen("types.txt", "r");
    if (file == NULL) 
    {
        printf("Can't open \"types.txt\"\n");
        exit(-1);
    }
    fscanf(file, "%d", &num_types);
    times = (time*) malloc(num_types*sizeof(time));
    for (i = 0; i < num_types; i++)
    {
        fscanf(file, "%d", &type);
        fscanf(file, "%d", &(times[type].wash));
        fscanf(file, "%d", &(times[type].dry));
    }
    fclose(file);
    
    //Инициализируем shared memory

    if ((key = ftok(path_name, 0)) < 0)
    {
        printf("Can\'t generate key\n");
        exit(-1);
    }

    if ((shmid = shmget(key, (N+2)*sizeof(int), 0666)) < 0)
    {
        printf("Can't create shared memory\n");
        exit(-1);
    }

    if ((shared = (int *)shmat(shmid, NULL, 0)) == (int *)(-1))
    {
        printf("Can't attach shared memory\n");
        exit(-1);
    }

    //Инициализируем семафоры

    if ((semid = semget(key, 3, 0666)) < 0)
    {
        printf("Can't create semaphore set\n");
            exit(-1);
    }

/*........... Основной код работы сушильщика...........*/

    while ((!shared[N+1]) || (shared[N]))
    {
	sem_operation(1, -1);
	sem_operation(2, -1);

	shared[N]--;
	i=0;
	while (shared[i] < 0) i++;
	plate = shared[i];
	shared[i] = -1;
	printf("Begin to dry plate of %d type\n", plate);
	if (shared[N])
	{
	    printf("There are %d plates on the table:", shared[N]);
	    for (i = 0; i < N; i++)
            if (shared[i] >= 0) printf(" %d", shared[i]);
	    printf("\n");
	}
	else
	    printf("There are no plates on the table\n");

	sem_operation(2, 1);
	sem_operation(0, 1);
	sleep(times[plate].dry);
	printf("The plate is dry\n");
    }

    //Отсоединяем разделяемую память

    if (shmdt(shared) < 0)
    {
        printf("Can't detach shared memory\n");
        exit(-1);
    }

    free(times);

    //Удаление разделяемой памяти из системы
    if (shmctl(shmid, IPC_RMID, NULL) < 0)
    {
        printf("Can't free shared memory\n");
        exit(-1);
    }

    //Удаление семафор
    if (semctl(semid, 0, IPC_RMID, 0) < 0)
    {
        printf("Can't free semaphores\n");
        exit(-1);
    }

    return 0;
}

void sem_operation(int num, int operation)
{
    bufer.sem_num = num;
    bufer.sem_op = operation;
    bufer.sem_flg = 0;

    if (semop(semid, &bufer, 1) < 0)
    {
        printf("Can't wait for condition\n");
        exit(-1);
    }
}
