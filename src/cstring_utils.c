#include "cstring_utils.h"
#include <stddef.h>
#include <string.h>

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