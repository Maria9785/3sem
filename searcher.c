#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

void find(char *name_file, int depth, char *name_dir)
{
    DIR *dir;
    struct dirent *object;

    struct stat buf;

    char way[1000];
    int len;
    
    way[0]='\0';
    strcpy(way, name_dir);
    strcat(way, "/");

    dir = opendir(name_dir);
    while ((object = readdir(dir)) != NULL)
    {
        if (!strcmp(object->d_name, ".") ||
            !strcmp(object->d_name, "..")) continue;
        strcat(way, object->d_name);
        len = strlen(object->d_name);
    

        if (stat(way, &buf) < 0)
        {
            printf("ERROR!\n");
            //exit(-1);
        }
        else
        {
            if (S_ISREG(buf.st_mode))
            {
                if (!strcmp(object->d_name, name_file))
                    printf("%s\n", way);
            }
            if (S_ISDIR(buf.st_mode))
            {
                if (depth > 0)
                    find(name_file, depth - 1, way);
            }
        }
        way[strlen(way)-len] = '\0';
    }

    closedir(dir);
}

int main(int argc, char *argv[])
{
    int depth;
    depth = atoi(argv[2]);

    find(argv[1], depth, argv[3]);

    return 0;
}
