#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
 * 1) Все магические числа вроде 20, 80 нужно заменить на константы
 * #define MAX_STRING_SIZE 80 и т.д.
 * 2) пробелы вокруг бинарных операторов (см. комментарий к abacaba)
 */

void split (char* string,char* delimeters, char*** tokens,int*tokensCount);

int main()
{
    char str[80];
    char delim[]={' ', '\t', '.', '\0'};
    int i;
    char** tok;
    int count;
    
    tok=(char**)malloc(20*sizeof(char*));
    
    printf("Enter string:\n");
    gets(str);
    split(str, delim, &tok, &count);
    for (i=0; i<count; i++)
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
