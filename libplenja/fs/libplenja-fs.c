#include <stdio.h>
#include "libplenja-fs.h"
#include<stdlib.h>
#include<string.h>

int writeFile(char *filename, fileLine *lines, long lineCount)
{
    FILE *file = fopen(filename, "w");
    if (file)
    {
        int i = 0;
        while (i < lineCount)
        {
            lines[i].chars = realloc(lines[i].chars, lines[i].length + 1);
            lines[i].chars[lines[i].length] = '\0';
            lines[i].length++;
            fputs(lines[i].chars, file);
            fputs("\n", file);

            i++;
        }

        fclose(file);
        return 0;
    }
    else
    {
        return 1;
    }
}

long openFile(char *filename, fileLine *out)
{
    FILE * file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("no file...");
        return -1;
    }
    
    char* currentline;
    size_t currentread;
    size_t len = 0;
    
    long linenumber = 0;
    
    out = malloc(countLines(filename) * sizeof(fileLine));
    //printf(" %d ", countlines(filename1));
    
    while ((currentread = getline(&currentline, &len, file)) != -1)
    {
        currentline[strlen(currentline) - 1] = '\0';
        //insertrow(linenumber, currentline);
        out[linenumber].chars = malloc(strlen(currentline));
        strncpy(out[linenumber].chars, currentline, strlen(currentline));
        out[linenumber].length = strlen(currentline);
        //free(currentline2);
        linenumber = linenumber + 1;
        //numberOfRows++;
    }
    fclose(file);
    free(currentline);

    return linenumber;
}

long countLines(char *filename)
{
    // count the number of lines in the file called filename                                    
    FILE *fp = fopen(filename,"r");
    long ch=0;
    long lines=0;

    if (fp == NULL)
    {
        return 0;
    }

    lines++;
    while(!feof(fp))
    {
        ch = fgetc(fp);
        if(ch == '\n')
        {
            lines++;
        }
    }
    fclose(fp);
    
    return lines;
}