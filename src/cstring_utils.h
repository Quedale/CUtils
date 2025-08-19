#ifndef C_STRING_H_ 
#define C_STRING_H_

int cstring_startsWith(const char *pre, const char *str);
int cstring_occurence_count(const char * text, int text_len, const char letter);
void cstring_safe_copy(char ** dest, char * source, char * default_val);

#endif
