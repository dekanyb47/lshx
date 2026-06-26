/*
Contatins functions related to flag arguments for builtin functions
*/

// TODO: add flags to other builtins (right now the only function to have flag arguments defined is lshx_ls)

// Usage: Each builtin that takes flag arguments has a local array containing flag structs, and variables corresponding to each flag.
// Parsing is done dynamically using the parse_flags function.
struct Flag {
    char *short_name;       // null terminated short name of the flag (e.g. -h)
    char *long_name;        // null terminated long name of the flag (e.g. --help)
    int takes_arg;          // determines if there is an expected argument after the flag (1) or not (0).
    void *value;            // the address of the variable defined in the given builtin function corresponding to the flag 
                            // (int if it doesn't take arguments, const char * if it does)
};

int is_flag(const char *flagstr);
int parse_flags(struct Flag *flags, int no_of_flags, const char **args, int args_start_i);
