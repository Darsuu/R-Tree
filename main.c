#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>

#define BUFFERLEN 100

typedef struct point{
    int x;
    int y;
}Point;



int main(void)
{
    FILE* fp = fopen("data.txt", "r");
    char* token;
    char tempBuffer[BUFFERLEN];

    while(fgets(tempBuffer, BUFFERLEN, fp)!=NULL)
    {
        token = strtok(tempBuffer," ");
        printf("x coordinate is %s\n", token);
        token = strtok(NULL, " ");
        printf("y coordinate is %s\n", token);
    }
    

    fclose(fp);
    return 0;
}


