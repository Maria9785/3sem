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

typedef struct wave
{
    int type;
    int amount;
} wave;

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
    int j = 0;
    int k;
    int N;

    //Работа с файлами 
    FILE *file;
    time *times;
    int num_types;
    int type;

    int num_waves;
    wave *waves;

    //Для работы с разделяемой памятью
    int *shared;
    int shmid;
    char path_name[] = "washer.c";
    key_t key;

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

    part_str += strlen(sub_string);
    N = atoi(part_str);

    //printf("%d\n", N);

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
    
    //Считываем количество посуды
    file = fopen("amount.txt", "r");
    if (file == NULL)
    {
        printf("Can't open \"amount.txt\"\n");
        exit(-1);
    }
    fscanf(file, "%d", &num_waves);
    waves = (wave*)malloc(num_waves*sizeof(wave));
    for (i = 0; i<num_waves; i++)
    {
        fscanf(file, "%d", &(waves[i].type));
        fscanf(file, "%d", &(waves[i].amount));
    }
    fclose(file);

    
    //Инициализируем shared memory

    if ((key = ftok(path_name, 0)) < 0)
    {
        printf("Can\'t generate key\n");
        exit(-1);
    }

    if ((shmid = shmget(key, (N+2)*sizeof(int), 0666 | IPC_CREAT)) < 0)
    {
        printf("Can't create shared memory\n");
        exit(-1);
    }

    if ((shared = (int *)shmat(shmid, NULL, 0)) == (int *)(-1))
    {
        printf("Can't attach shared memory\n");
        exit(-1);
    }

    for (i = 0; i < N; i++)
    shared[i] = -1;
    shared[N] = 0;
    shared[N+1] = 0;

    //Инициализируем семафоры

    if ((semid = semget(key, 3, 0666 | IPC_CREAT)) < 0)
    {
        printf("Can't create semaphore set\n");
        exit(-1);
    }

    sem_operation(0, N);
    sem_operation(2, 1);

/*........... Основной код работы мойщика...........*/

    for (i = 0; i < num_waves; i++)
    {
	for (j = 0; j < waves[i].amount; j++)
	{
	    sem_operation(0, -1);
	    printf("Began to wash plate of type %d\n", waves[i].type);
	    sleep(times[waves[i].type].wash);
	    sem_operation(2, -1);
	    shared[N]++;

	    //Ищем пустое место на столе
	    k = 0;
	    while (shared[k] >= 0) k++;
	    shared[k] = waves[i].type;
	    printf("The plate of type %d is on the table\n", waves[i].type);
	    printf("There are %d plates on the table:", shared[N]);
	    for (k = 0; k < N; k++)
            if (shared[k] >= 0) printf(" %d", shared[k]);
	    printf("\n");
	    sem_operation(2, 1);
	    
	    sem_operation(1, 1);
	}
    }
    shared[N+1] = 1; //сообщаем что тарелки закончились

    //Отсоединяем разделяемую память

    if (shmdt(shared) < 0)
    {
        printf("Can't detach shared memory\n");
        exit(-1);
    }

    free(times);
    free(waves);

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
