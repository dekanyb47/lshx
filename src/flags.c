#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "flags.h"

int is_flag(const char *flagstr) {
    size_t len = strlen(flagstr);
    if (len == 2 && flagstr[0] == '-' && isalpha(flagstr[1])) return 1;     // -x format 
    else if (len > 2 && flagstr[0] == '-' && flagstr[1] == '-') {           // --xxx format
        for (size_t i = 2; i < len; i++) {
            if (!isalpha(flagstr[i])) return 0;
        }
        return 1;
    }
    return 0;
}

// Parses input arguments, and dynamically modifies values corresponding to the flags found. Returns -1 on failure or incorrect input.
// See the struct definition for Flag for more information.
int parse_flags(struct Flag *flags, int no_of_flags, const char **args, int args_parse_start_i) {
    if (args == NULL || no_of_flags <= 0 || args_parse_start_i < 0) {
        return -1;
    }
    int args_len = 0;
    while(args[args_len] != NULL) {
        args_len++;
    }
    if (args_len < args_parse_start_i) {      // args_len can be equal to the args_parse_start_i because the last element in it is NULL
        return -1;
    }

    for (int i = args_parse_start_i; i < args_len; i++) {
        if (flags == NULL) {
            if(is_flag(args[i])) return -1;
            continue;
        }

        int is_recognized_flag = 0;
        for (int j = 0; j < no_of_flags; j++) {
            if (strcmp(args[i], flags[j].long_name) == 0 ||
                strcmp(args[i], flags[j].short_name) == 0) {
                if (flags[j].takes_arg == 1) {
                    if (args[i + 1] == NULL || is_flag(args[i + 1])) {      // if no arguments were given
                        return -1;
                    }
                    *(const char **)flags[j].value = args[i + 1];   // changes the value of the char pointer variable flag.value points to.
                }
                else {
                    *(int *)flags[j].value = 1;                     // changes the value of the int variable flag.value points to.
                }
                is_recognized_flag = 1;
                break;
            }
        }
        if (!is_recognized_flag && is_flag(args[i])) return -1;       // return when an unrecognized flag was found
    }

    return 0;
}