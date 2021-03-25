#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define COMMAND "sequential_min_max"

int main(int argc, char *argv[])
{
    int pid = fork();
    if (pid > 0)
    {
        execvp("./" COMMAND, argv);
    }
    return 0;
}