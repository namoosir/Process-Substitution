// Add necessary #include's
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>


int procsub(const char *prog1, const char *prog2, char *prog2_argv[], int status[])
{
    int fd[2];
    pid_t child1;
    pid_t child2;

    if (pipe(fd) != 0) {    //1
        return -1;
    }

    child1 = fork();

    if (child1 == -1) {  //2
        return -1;
    }
    else if (child1 == 0) {
        close(fd[1]);       //closed writing
        char reader[1024];                

        sprintf(reader, "/dev/fd/%d", fd[0]);               
  
        execlp(prog1, prog1, reader, NULL);
        exit(37);
    }
    else {
        child2 = fork();

        if (child2 == -1) {
            return -1;
        }
        else if (child2 == 0) {
            close(fd[0]);       //closed reading
            char writer[1024];
            sprintf(writer, "/dev/fd/%d", fd[1]);

            dup2(fd[1], fileno(stdout));

            close(fd[1]);

            execvp(prog2, prog2_argv);
            exit(37);
        }
        else {
            close(fd[0]);
            close(fd[1]);

            for (int i = 0; i < 2; i++) {
                int st;
                pid_t temp;

                temp = wait(&st);

                if (temp == child1) {
                    status[0] = (st);
                }
                else if (temp == child2) {
                    status[1] = (st);
                }
            }

            if (status[0] == 0 && status[1] == 0) {
                return 0;
            }
        }

    }
}
