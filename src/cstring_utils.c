#include "cstring_utils.h"
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

int cstring_startsWith(const char *pre, const char *str)
{
    size_t lenpre = strlen(pre),
           lenstr = strlen(str);
    return lenstr < lenpre ? 0 : memcmp(pre, str, lenpre) == 0;
}

int cstring_occurence_count(const char * text, int text_len, const char letter){
    int count =0;
    for(int i = 0; i<text_len; i++){
    	if(text[i] == letter){
          count++;
        }
    }
    return count;
}

void cstring_safe_copy(char ** dest, char * source, char * default_val){
    if(source){
        *dest = malloc(strlen(source)+1);
        strcpy(*dest,source);
    } else if(default_val){
        *dest = malloc(strlen(default_val)+1);
        strcpy(*dest,default_val);
    } else {
        *dest = NULL;
    }
}
