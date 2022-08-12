#ifndef PLENJA_FS_H
#define PLENJA_FS_H

typedef struct fileLine {
    char *chars;
    long length;
} fileLine;

int writeFile(char *filename, fileLine *lines, long lineCount);
long openFile(char *filename, fileLine *lines);
long countLines(char *filename);

#endif