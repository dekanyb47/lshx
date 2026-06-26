#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "run.h"
#include "builtins.h"

int lshx_launch(const char **args) {
    pid_t pid, wpid;
    int status;

    pid = fork();
    if (pid == 0) {
        // Child process
        if (execvp(args[0], (char * const *)args) == -1) {
            perror("lshx");
            exit(1);
        }
    }
    else if (pid < 0) {
        // Error forking
        perror("lshx");
    } else {
        // Parent process
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

int lshx_execute(const char **args) {
    size_t i;

    if (args[0] == NULL) {
        // Empty command
        return 1;
    }

    for (i = 0; i < lshx_num_builtins(); i++) {
        if (strcmp(args[0], builtins[i].str) == 0) {
            return (*builtins[i].func)(args);
        }
    }

    return lshx_launch(args);
}
