#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "input.h"

#define LSHX_RL_BUFSIZE 1024
#define LSHX_TOK_BUFSIZE 64
#define LSHX_TOK_DELIM " \t\r\n\a"

#define LSHX_QUOTATION_MARKS "\"'"


// TODO: take quotation marks followed by backslash as a literal char
// TODO: issue: inputs like 'foo'"bar" are tokenized as bar

// Helper function for lshx_tokenize
void lshx_tokenize_quotation_marks(char *line, const size_t i, char **p_curr_token, int *p_is_open_quotation, char *p_quotation_type) {
    const size_t line_len = strlen(line);

    if (strchr(LSHX_QUOTATION_MARKS, line[i])) {
        if (!(*p_is_open_quotation)) {
            *p_is_open_quotation = 1;
            *p_quotation_type = line[i];
            *p_curr_token = &line[i] + 1;              // start the next token from after the quotation mark
            line[i] = '\0';                            // replace the quotation mark with a null-terminator
        } else if ((*p_is_open_quotation) && line[i] == *p_quotation_type) {
            *p_is_open_quotation = 0;
            if (i == line_len - 1 ||                       // quotation mark is replaced with a null terminator when it is the last char in the input
                strchr(LSHX_TOK_DELIM, line[i + 1])) {     // or when the next char is a delimiter
                line[i] = '\0';
            }
        }
    }
}

// Helper function for lshx_tokenize
int lshx_tokenize_increase_bufsize(const char ***p_tokens, size_t *p_tokens_bufsize) {
    *p_tokens_bufsize += LSHX_TOK_BUFSIZE;
    *p_tokens = realloc(*p_tokens, *p_tokens_bufsize * sizeof(char *));
    if (*p_tokens == NULL) {
        fprintf(stderr, "lshx: allocation error\n");
        return -1;
    }
    return 0;
}

// TODO: create tokenlist struct to make it cleaner
// Helper function for lshx_tokenize
void lshx_tokenize_delimiters(char *line, size_t i, const char ***p_tokens, size_t *p_tokens_bufsize, size_t *p_token_pos, char **p_curr_token) {    
    if (strchr(LSHX_TOK_DELIM, line[i])) {
        line[i] = '\0';

        if (**p_curr_token != '\0') {                 // skip empty tokens (happens when there are multiple delimiters next to one another)
            (*p_tokens)[(*p_token_pos)++] = *p_curr_token;
            if (*p_token_pos >= *p_tokens_bufsize - 2) {        // check if there is enough space, leaving space for last token and NULL padding
                if (lshx_tokenize_increase_bufsize(p_tokens, p_tokens_bufsize) != 0) {
                    fprintf(stderr, "lshx: allocation error\n");
                    exit(1);
                };
            }
        }
        *p_curr_token = &line[i] + 1;
    }
}

// Splits a line of text to tokens by replacing delimiters with null terminators inside line,
// and puts them in an array of strings allocated on the heap. Text between quotation marks is kept as a literal.
// The last element in the return array is NULL.
// Takes a null terminated line allocated on the heap as the argument.
const char **lshx_tokenize(char *line) {
    const size_t line_len = strlen(line);

    size_t tokens_bufsize = LSHX_TOK_BUFSIZE;
    const char **tokens = malloc(tokens_bufsize * sizeof(char *));
    size_t token_pos = 0;
    char *curr_token = line;    // points to the current token inside line

    int is_open_quotation = 0;
    char quotation_type = '\0';

    if (tokens == NULL) {
        fprintf(stderr, "lshx: allocation error\n");
        exit(1);
    }

    for (size_t i = 0; i < line_len; i++) {
        lshx_tokenize_quotation_marks(line, i, &curr_token, &is_open_quotation, &quotation_type);
        if (is_open_quotation) continue;
        lshx_tokenize_delimiters(line, i, &tokens, &tokens_bufsize, &token_pos, &curr_token);       
    }

    if (*curr_token != '\0') tokens[token_pos++] = curr_token;      // add the last token
    tokens[token_pos] = NULL;
    return tokens;
}

char *lshx_readline() {
    int bufsize = LSHX_RL_BUFSIZE;
    int pos = 0;
    char *buf = malloc(bufsize * sizeof(char));
    int c;

    if (buf == NULL) {
        fprintf(stderr, "lshx: allocation failed\n");
        exit(1);
    }

    while (1) {
        c = getchar();

        if (c == EOF || c == '\n') {
            buf[pos] = '\0';
            return buf;
        } else {
            buf[pos] = c;
            pos++;
        }

        if (pos >= bufsize) {
            bufsize += LSHX_RL_BUFSIZE;
            buf = realloc(buf, bufsize);
            if (buf == NULL) {
                fprintf(stderr, "lshx: Reallocation failed\n");
                exit(1);
            }
        }
    }
}

// Unused function, originally in lsh
char **lshx_split_line(char *line) {
    int bufsize = LSHX_TOK_BUFSIZE;
    int position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;

    if (tokens == NULL) {
        fprintf(stderr, "lshx: allocation error\n");
        exit(1);
    }

    token = strtok(line, LSHX_TOK_DELIM);
    while(token != NULL) {
        tokens[position] = token;
        position++;

        if (position >= bufsize) {
            bufsize += LSHX_TOK_BUFSIZE;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if (tokens == NULL)  {
                fprintf(stderr, "lshx: allocation error\n");
                exit(1);
            }
        }

        token = strtok(NULL, LSHX_TOK_DELIM);
    }
    
    tokens[position] = NULL;
    return tokens;
}

