#ifndef PLENJA_H_
#define PLENJA_H_

//typedef struct fileLine; fileLine;
typedef struct fileLine {
    char *chars;
    long length;
} fileLine;

int writeFile(char *filename, fileLine *lines, long lineCount);
long openFile(char *filename, fileLine *lines);
long countLines(char *filename);
char* insert_char_malloc (char *str, int len, char c, int pos);

#endif