

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_NUMBER_OF_SYMBOLS 80
#define MAX_NUMBER_OF_TOKENS 20
#define MAX_NUM_SYM_IN_WORD 30

void split (char* string, char* delimeters, char*** tokens, int* tokensCount);

int main(int argc, const char * argv[]) {
    FILE* file = fopen ("file.txt", "r");
    int num_of_strings;
    int i, j;
    char string[MAX_NUMBER_OF_SYMBOLS];
    char delim[]={' ', '\n', '\0'};
    char*** tok;
    int* count;
    pid_t pid;
    
    
    fscanf(file, "%d", &num_of_strings);
    
    tok = (char***)malloc(num_of_strings * sizeof(char**));
    for (i = 0; i< num_of_strings; i++)
        tok[i] = (char**)malloc(MAX_NUMBER_OF_TOKENS * sizeof(char*));
    count = (int*)malloc(num_of_strings * sizeof(int));
    
    fgets(string, MAX_NUMBER_OF_SYMBOLS, file);
    for (i = 0; i < num_of_strings; i++)
    {
        fgets(string, MAX_NUMBER_OF_SYMBOLS, file);
        split(string, delim, &(tok[i]), &(count[i]));
        tok[i][count[i]] = NULL;
    }
    
    fclose(file);
    
    for (i = 0; i < num_of_strings; i++)
    {
        pid = fork();
        if (pid == 0)
        {
            sleep(atoi(tok[i][0]));
            execvp(tok[i][1], tok[i]+1);
            exit(0);
        }
    }
    
    for (i = 0; i < num_of_strings; i++)
        for (j = 0; j < count[i]; j++)
            free(tok[i][j]);
    for (i=0; i<num_of_strings; i++)
        free(tok[i]);
    free(tok);
    free(count);
    
    return 0;
}
    
void split (char* string, char* delimeters, char*** tokens, int* tokensCount)
{
    *tokensCount = 0;
    char *pch = strtok(string, delimeters);
    while (pch != NULL)
    {
        (*tokens)[*tokensCount] = (char*) malloc(MAX_NUM_SYM_IN_WORD * sizeof(char));
        memcpy((*tokens)[*tokensCount], pch, strlen(pch)+1);
        (*tokensCount)++;
        pch =strtok(NULL, delimeters);
    }
    free(pch);
}

