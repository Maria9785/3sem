#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_STRING_SIZE 80
#define MAX_NUMBER_TOK 20

void split (char* string, char* delimeters, char*** tokens, int* tokensCount);

int main()
{
    char str[MAX_STRING_SIZE];
    char delim[] = {' ', '\t', '.', '\0'};

void split (char* string,char* delimeters, char*** tokens,int*tokensCount);

int main()
{
    char str[80];
    char delim[]={' ', '\t', '.', '\0'};
    int i;
    char** tok;
    int count;
    
    tok = (char**)malloc(MAX_NUMBER_TOK * sizeof(char*));
    
    printf("Enter string:\n");
    gets(str);
    split(str, delim, &tok, &count);
    for (i = 0; i < count; i++)
        printf("%s\n", tok[i]);
    printf("numder of words: %d", count);
    free(tok);
    return 0;
}

void split (char* string,char* delimeters, char*** tokens,int*tokensCount)
{
    *tokensCount = 0;
    char *pch = strtok(string, delimeters);
    while (pch != NULL)
    {
        (*tokens)[*tokensCount] = pch;
        (*tokensCount)++;
        pch =strtok(NULL, delimeters);
    }
}
