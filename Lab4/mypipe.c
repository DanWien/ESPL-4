#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h> 

int main( int argc, char **argv)
{
    int fd[2];
    if(pipe(fd)==-1){
        printf("pipe failed");
        exit(1);
    }
    int pid=fork();
    if(pid==-1){
        printf("fork failed");
        exit(1);
    }
    else if(pid==0){
        close(fd[0]);
        write(fd[1], "hello",5);
        close(fd[1]);
        exit(0);
    }
    else{
        close(fd[1]);
        char msg[1000];
        read(fd[0],msg,sizeof(msg));
        msg[5]='\0';
        printf("msg from child: %s\n",msg);
        close(fd[0]);
        wait(NULL);
    }
    return 0;
}