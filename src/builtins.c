#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#include "builtins.h"
#include "flags.h"
#include "helper.h"

const struct BuiltinCommand builtins[] = {
    {"cd",    &lshx_cd,    "Change the current working directory. Usage: cd [dir_path]"},
    {"exit",  &lshx_exit,  "Exit the shell"},
    {"help",  &lshx_help,  "List available builtin commands"},
    {"ls",    &lshx_ls,    "List files and directories in the current directory. Usage: ls [-d|-f]"},
    {"mkdir", &lshx_mkdir, "Create a new directory. By default, it gives read/write/search permissions for owner and group, and read/search permissions for others. Usage: mkdir [dir_path]"},
    {"mv",    &lshx_mv,    "Move or rename a file or directory. Usage: mv [src_path] [dest_path]"},
    {"stat",  &lshx_stat,  "Display file or directory status information. Usage: stat [path]"},
};
#define LSHX_NUM_BUILTINS sizeof(builtins) / sizeof(builtins[0])

size_t lshx_num_builtins() {
    return LSHX_NUM_BUILTINS;
}

void lshx_print_builtin_helpmsg(char *func_keyword_str) {
    for (size_t i = 0; i < LSHX_NUM_BUILTINS; i++) {
        if (builtins[i].str == func_keyword_str) {
            printf("%s: %s\n", func_keyword_str, builtins[i].help_msg);
            return;
        }
    }
    fprintf(stderr, "lshx: Help message for builtin funciton '%s' not found.", func_keyword_str);
    return;
}

int lshx_cd(const char **args) {
    if (args[1] == NULL) {
        lshx_print_builtin_helpmsg("cd");
        return 1;
    }

    if (chdir(args[1]) != 0) {
        perror("lshx");
    }
    return 1;
}

int lshx_help(const char **args) {
    printf("lshx: Shell implementation based on Stephen Brennan's LSH\n");
    printf("Type program names and arguments, and hit enter.\n");
    printf("The following are built in:\n");

    for (size_t i = 0; i < lshx_num_builtins(); i++) {
        printf("  %s\n", builtins[i].str);
    }

    printf("Use the man command for information on other programs.\n");
    return 1;
}

int lshx_exit(const char **args) {
    return 0;
}

int lshx_mv(const char **args) {
    char conf;

    if (args[1] == NULL || args[2] == NULL) {
        lshx_print_builtin_helpmsg("mv");
        return 1;
    } 

    if (access(args[1], F_OK) != 0) {
        fprintf(stderr, "lshx: source file doesn't exist\n");
        return 1;
    }

    if (access(args[2], F_OK) == 0) {           // confirmation if file exists at dest
        printf("File already exists at specified path.\n");
        printf("Replace \"%s\" with \"%s\"? (y/n): ", args[1], args[2]);

        conf = getchar();    

        // clear stdin in case more characters were inputted
        int c;
        while ((c = getchar()) != '\n' && c != EOF);

        if (tolower(conf) != 'y') {
            return 1;
        }
    }

    if (rename(args[1], args[2]) == -1) {
        perror("lshx");
    }
    return 1;
}

int lshx_mkdir(const char **args) {
    if (args[1] == NULL) {
        lshx_print_builtin_helpmsg("mkdir");
        return 1;
    }

    if (access(args[1], F_OK) == 0) {       // if a file or directory already exists at the location
        struct stat st;
        if (stat(args[1], &st) != 0) {      // read the stat of existing file/directory to st
            perror("lshx");
            return 1;
        }

        if (st.st_mode & __S_IFDIR) {
            fprintf(stderr, "lshx: directory of the same name already exists\n");
            return 1;
        }
    }

    if (mkdir(args[1], S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0) {      // read/write/search permissions for owner and group, and with read/search permissions for others.
        perror("lshx");
        return 1;
    }

    return 1;
}

int lshx_ls(const char **args) {
    int directories_only = 0;
    int files_only = 0;
    const char *test = NULL;       // temporary flag to test flag argument parsing

    struct Flag flags[] = {
        {"-d", "--directories", 0, &directories_only},
        {"-f", "--files", 0, &files_only},
        {"-t", "--test", 1, &test}
    };
    const int no_of_flags = sizeof(flags) / sizeof(flags[0]);

    if (parse_flags(&flags[0], no_of_flags, args, 1) != 0 ||      // failed to parse flags
        (directories_only == 1 && files_only == 1)) {             // incorrect usage of flags
        lshx_print_builtin_helpmsg("ls");
        return 1;
    }

    // test flag
    if (test != NULL) printf("Flag argument read from -t: %s\n", test);

    char cwd_buf[256];
    if (getcwd(cwd_buf, sizeof(cwd_buf)) == NULL) {
        perror("lshx");
        return 1;
    }
        
    DIR *cwdp = opendir(cwd_buf);
    if (cwdp == NULL) {
        perror("lshx");
        return 1;
    }

    struct stat st;
    struct dirent *entry = readdir(cwdp);
    while (entry != NULL) {
        if (directories_only == 0 && files_only == 0) {     // if there are no specifications
            printf("%s\n", entry->d_name);
        }
        else {
            if (stat(entry->d_name, &st) != 0) {
                entry = readdir(cwdp);              // if readdir fails, the program breaks from the loop (returns NULL).
                continue;
            }
            if ((directories_only == 1 && st.st_mode & __S_IFDIR) ||               // when it is a directory with -d flag specified
                (files_only == 1 && st.st_mode & __S_IFREG)) {                     // when it is a file with -f flag specified
                printf("%s\n", entry->d_name);
            }
        }
        entry = readdir(cwdp);
    }
    printf("\n");

    return 1;
}  

int lshx_stat(const char **args) {
    if (args[1] == NULL) {
        lshx_print_builtin_helpmsg("stat");
        return 1;
    }

    struct stat *stat_buf = malloc(sizeof(struct stat));
    const size_t time_bufsize = 128;
    char *time_buf = malloc(time_bufsize);

    if (stat_buf == NULL || time_buf == NULL) {
        fprintf(stderr, "lshx: allocation error\n");
        exit(1);
    }

    if (stat(args[1], stat_buf) != 0) {
        perror("lshx");
        return 1;
    }

    printf("File: %s\n", args[1]);
    printf("Size: %ldB,\tNumber of blocks: %ld,\tIO Block: %ld\n", stat_buf->st_size, stat_buf->st_blocks, stat_buf->st_blksize);
    printf("Access: %ld,\tUser ID: %d,\tGroup ID: %d\n", stat_buf->st_dev, stat_buf->st_uid, stat_buf->st_gid);

    hrf_time(time_buf, time_bufsize, stat_buf->st_atime);
    printf("Last accessed: %s\n", time_buf);
    hrf_time(time_buf, time_bufsize, stat_buf->st_mtime);
    printf("Last modified: %s\n", time_buf);
    hrf_time(time_buf, time_bufsize, stat_buf->st_ctime);
    printf("Last status change: %s\n", time_buf);

    free(stat_buf);
    free(time_buf);

    return 1;
}
