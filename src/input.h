/*
Contains functions related taking and parsing user input
*/

void lshx_tokenize_delimiters(char *line, size_t i, const char ***p_tokens, size_t *p_tokens_bufsize, size_t *p_token_pos, char **p_curr_token);
void lshx_tokenize_quotation_marks(char *line, const size_t i, char **p_curr_token, int *p_is_open_quotation, char *p_quotation_type);
int lshx_tokenize_increase_bufsize(const char ***p_tokens, size_t *p_tokens_bufsize);
const char **lshx_tokenize(char *line);

char **lshx_split_line(char *line);
char *lshx_readline();
