#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * 1) вокруг бинарных операторов ставьте пробелы: "i + j", а не "i+j".
 * 2) после , и ; ставьет пробел: 
 * for (i = 1; i < n; i++)
 * (int n, char* string)
 */

void GenerateString (int n,char* string)
{
    int i;
    int len;
    string[0]='a';
    len=strlen(string);
    for (i=1;i<n;i++)
    {
        string[len]=string[0]+i;
        strncpy(string+len+1,string,len);
        len= strlen(string);
    }
    printf("%s",string);
}
int main()
{
    int n;
    char* string;
    string=(char*) malloc((1<<n)*sizeof(char));
    scanf("%d",&n);
    GenerateString (n,string);
    return 0;
}
