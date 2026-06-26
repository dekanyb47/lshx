/*
lshx
A simple shell implementation based on Stephen Brennan's lsh.
Made by dekanyb47 
2026
*/

#include <stdio.h>
#include <stdlib.h>

#include "builtins.h"
#include "input.h"
#include "flags.h"
#include "run.h"

void lshx_loop() {
    int status;

    do {
        printf("> ");
        char *line = lshx_readline();
        const char **args = lshx_tokenize(line);
        status = lshx_execute(args);

        free(line);
        free(args);
    } while (status);
}

int main() {
    // load files

    // start loop
    lshx_loop();    

    // cleanup, shut down
    
    return 0;
}