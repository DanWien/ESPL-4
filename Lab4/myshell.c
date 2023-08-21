#include <linux/limits.h>
#include "LineParser.c"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>  // for file related system calls (open)
#include <sys/types.h>  // for system calls related to types
#include <sys/stat.h>  // for file permission bits
#include <sys/wait.h>  // for waitpid system call


int debug = 0;


void execute(cmdLine *pCmdLine) {
    int pid=fork();
    if(pid==-1){
        printf("fork failed\n");
        exit(1);
    }
    else if(pid==0){
        if (pCmdLine->inputRedirect != NULL) {
            int input_fd = open(pCmdLine->inputRedirect, O_RDONLY);
            if (input_fd == -1) {
                perror("open input file failed\n");
                _exit(1);
            }
            if (dup2(input_fd, 0) == -1) {
                perror("dup2 input failed\n");
                _exit(1);
            }
            close(input_fd);
        }
        if (pCmdLine->outputRedirect != NULL) {
            int output_fd = open(pCmdLine->outputRedirect, O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
            if (output_fd == -1) {
                perror("open output file failed\n");
                _exit(1);
            }

            if (dup2(output_fd, 1) == -1) {
                perror("dup2 output failed\n");
                _exit(1);
            }
            close(output_fd);
        }

        if(execvp(pCmdLine->arguments[0], pCmdLine->arguments)==-1){
            perror("execvp failed\n");
            exit(1);
        }
    }

    else {
        if(debug) // parent process
            fprintf(stderr, "PID: %d\nExecuting command: %s\n", pid, pCmdLine->arguments[0]);    // 1b
        if (pCmdLine->blocking == 1) {
            int status;
            while(waitpid(pid, &status, 0)>0)
                printf("waiting for childs' process\n");
            printf("finished childs' process\n");
        }
    }
}



int main(int argc, char **argv)
{
    int pid;
    if (argc>1) {
        for(int i = 1 ; i<argc ; i++){
            if(strcmp(argv[i] , "-d") == 0)
                debug = 1;
        }
    }
    while (1) {
        char pathName[PATH_MAX];
        getcwd(pathName, sizeof(pathName));
        printf("current working directory: %s\n", pathName);
        char input[2048];
        printf("\nenter command lines: ");
        if(fgets(input,sizeof(input),stdin)<=0)
            printf("couln't get command lines\n");

        input[strcspn(input,"\n")]='\0';

        if(strncmp(input,"quit" , 4)==0)
            exit(0);

        else if(strncmp(input, "cd",2) == 0) {   // 1c
            if(chdir(input+3) == -1)
                fprintf(stderr, "cd failed\n");
        }

        else if(strncmp(input, "suspend",7) == 0) {
            pid=atoi(input+8);
            if (kill(pid, SIGTSTP) == -1) {
                perror("kill(SIGTSTP) failed\n");
                return 1;
            }
            printf("Process with PID %d suspended\n", pid);
        }

        else if (strncmp(input, "wake",4) == 0) {
            pid=atoi(input+5);
            if (kill(pid, SIGCONT) == -1) {
                perror("kill(SIGCONT) failed\n");
                return 1;
            }
            printf("Process with PID %d woke up\n", pid);
        }

        else if (strncmp(input, "kill",4) == 0) {
            pid=atoi(input+5);
            if (kill(pid, SIGKILL) == -1) {
                perror("kill(SIGKILL) failed\n");
                return 1;
            }
            printf("Process with PID %d killed\n", pid);
        }

        cmdLine * line = parseCmdLines(input);
        execute(line);
        freeCmdLines(line);
    }
    return 0;
}


