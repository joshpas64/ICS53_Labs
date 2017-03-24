#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
/* Submitter: Joshua Pascascio
// UCInetid: jpascasc
// Id#: 52192782
*/
void writePipe(int pipe, int arg);

void writePipe(int pipe, int arg){
    FILE* stream;
    stream = fdopen(pipe,"w");
    fprintf(stream,"%d\n",arg);
    fclose(stream);
}

int readPipe(int pipe);

int readPipe(int pipe){
    FILE* stream;
    stream = fdopen(pipe,"r");
    char line[20];
    char linecpy[21];
    int pipeResult;
    while(fgets(line,21,stream))
        strcpy(linecpy,line);
    fclose(stream);
    sscanf(linecpy,"%d",&pipeResult);
    return pipeResult;
}



int pipeSearch(int* arr, int check, int start, int end);

int pipeSearch(int* arr, int check, int start, int end){
    if((end - start) == 1){
        if(arr[start] == check)
            return start;
        return -1;
    }
    int result, status;
    int mypipe[2];
    int midpoint = end - start;
    if(midpoint % 2 == 1)
        midpoint++;
    midpoint = midpoint / 2;
    pipe(mypipe);
    pid_t pid = fork();
    if(pid == 0){
        close(mypipe[0]);
        int temp = pipeSearch(arr,check,start, start + midpoint);
        writePipe(mypipe[1],temp);
        exit(0);
    }else{
        wait(&status);
        close(mypipe[1]);
        result = readPipe(mypipe[0]);
        if(result == -1)
            result = pipeSearch(arr,check,start + midpoint, end);
        return result;
    }
}

FILE* loadfile(int* search);

FILE* loadfile(int* search){
    FILE* file;
    printf("$  ");
    char command[20];
    char fileName[20];
    scanf("%s %s %d",command,fileName,search);
    file = fopen(fileName,"r");
    while(strcmp(command,"MySearch") != 0 || file == NULL || search == NULL){
        printf("Sorry you must load a valid command or file first\n$  ");
        if(file != NULL)
            fclose(file);
        scanf("%s %s %d",command,fileName,search);
        file = fopen(fileName,"r");
    }
    return file;
}

int parseArray(FILE* file, int* length, int* arr);

int parseArray(FILE* file, int* length, int* arr){
    int count = 0;
    char line[255];
    fgets(line,255,file);
    fclose(file);
    strcpy(line,line);
    char* tokens = strtok(line," ");
    while(tokens != NULL){
        if(count == 10)
            return 1;
        *(arr + count) = atoi(tokens);
        tokens = strtok(NULL," ");
        count++;
    }
    *length = count;
    return 0;
}


int main()
{
    int searchKey;
    FILE* file = loadfile(&searchKey);
    int length;
    int returnArray[10];
    while(parseArray(file, &length,returnArray) == 1){
        printf("That file had an invalid format try again\n");
        FILE* file = loadfile(&searchKey);
    }
    int forkResult = pipeSearch(returnArray,searchKey,0,length);
    printf("%d\n",forkResult);
    forkResult = pipeSearch(returnArray,searchKey,0,length);
    return 0;
}
