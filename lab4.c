#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

void unix_error(char * msg)
{
    fprintf(stderr, "%s: %s:\n", msg, strerror(errno));
    exit(0);
}

int main(int argc, char **argv)
{
        int aPipe[2]; // set up pipe
        if(pipe(aPipe) == -1)
        {
            perror("Pipe error");
            exit(EXIT_FAILURE);
        }
        pid_t pid;
        if((pid = fork()) < 0)
            unix_error("fork error");
        int sum = 0;
        int i = 0;
        if(pid == 0) // -- running in child process --
        {
            int curr = 0;
            int bytes = 0;
            if(close(aPipe[1]) == -1)
            {
                printf("Pipe write error");
                exit(EXIT_FAILURE);
            }
            while(bytes = read(aPipe[0], &curr, sizeof(int)) > 0)
            {
                sum += curr;
            }
            if(bytes == -1)
            {
                printf("pipe read error");
                exit(EXIT_FAILURE);
            }
            if(close(aPipe[0]) == -1)
            {
                printf("pipe read close error");
                exit(EXIT_FAILURE);
            }
            return sum;
        }
        else// -- running in parent process --
        {
            int status = 0;
            int bit = 0;
            int length = argc - 1;
            int * arr;
            arr = (int*) malloc((argc - 1) * sizeof(int));
            if(arr == NULL)
            {
                printf("memory allocation error");
                exit(0);
            }
            for(i = 0; i < length; i += 1)
            {
                *(arr+i) = atoi(argv[i+1]);
            }
            if(close(aPipe[0]) == -1)
            {
                printf("pipe read close error");
                exit(EXIT_FAILURE);
            }
            for(i = 0; i < length; i += 1)
            {
                bit = write(aPipe[1], &arr[i], sizeof(int));
                if(bit == 1)
                {
                    printf("pipe write error");
                    exit(EXIT_FAILURE);
                }
            }
            if(close(aPipe[1]) == -1)
            {
                printf("pipe write close error");
                exit(EXIT_FAILURE);
            }
            free(arr);
            pid_t waitStatus = waitpid(pid, &status, 0);
            if(waitStatus == -1)
            {
                printf("waitpid error");
                exit(-1);
            }
            if(WIFEXITED(status))
            {
                sum = WEXITSTATUS(status);
            }
	printf("CS201 - Assignment 3 Regular - Devon Wilson\n");
        printf("sum = %d\n", sum);
	return 0;
	}
}
