#include "libplenja-str.h"
#include <stdio.h>
#include<stdlib.h>
#include<string.h>

char* insert_char_malloc (char *str, int len, char c, int pos)
{
    char *p;
    int i;

    p = malloc(len + 2);
    for (i = 0 ; i < pos ; ++i)
    p[i] = str[i];
    p[i++] = c;
    for ( ; i < len + 2 ; ++i)
    p[i] = str[i - 1];
    free(str);
    return p;
}