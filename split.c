#include <stdio.h>
#include <string.h>

void   split (char* string,char* delimeters, char*** tokens,int*tokensCount);
int main()
{
    char str[80];
    char delim[]={' ', '\t', '.', '\0' };
    int i;
    char** tok;
    int count;
    
    printf("Enter string:\n");
    gets(str);
    split(str, delim, &tok, &count);
    for (i=0; i<count; i++)
        printf("%s\n", tok[i]);
    printf("numder of words: %d", count);
    return 0;
}
void   split (char* string,char* delimeters, char*** tokens,int*tokensCount)
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
