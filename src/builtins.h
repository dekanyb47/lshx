/*
Contains:
 - builtins defined in the global array builtins
 - functions for the builtins
*/

struct BuiltinCommand {
    char *str;                      // null-terminated argument for the builtin (e.g. 'cd')
    int (*func)(const char **);     // function pointer to the respective command
    char *help_msg;                 // null-terminated help message
};

extern const struct BuiltinCommand builtins[];

size_t lshx_num_builtins();
void lshx_print_builtin_helpmsg(char *func_keyword_str);
int lshx_cd(const char **args);
int lshx_exit(const char **args);
int lshx_help(const char **args);
int lshx_ls(const char **args);
int lshx_mkdir(const char **args);
int lshx_mv(const char **args);
int lshx_stat(const char **args);
