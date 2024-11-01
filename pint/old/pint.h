#ifndef PINT_H_
#define PINT_H_

#include<libplenja.h>

int getsize(int *x, int *y);
void writespecificline(long line, pstring chars);
int edit(char filename1[]);
void writespecificlinewithcolor(long line, pstring chars, int textcolor, int backgroundcolor);
//int countlines(char *filename);

int insertrow(long insertbeforeindex, pstring row);
int insertchar(long row, long insertbeforeindex, char insertchar);
//int expandrow(long row);
int expandrows();
int drawlinenocopy(long row, long offset);
int drawline(long row, pstring line, long offset);
void writespecificcharswithcolor(long line, long charindex, char *chars, int textcolor, int backgroundcolor);
int removerow(long index);
int deletechar(long row, long deleteindex);

int calculateoffset(long row, bool movingright);
int calculatetabwidth(long index);//, bool realpos);
int processchar(int sizeY, int sizeX, long charin);
int stdmessage(int sizeY, int sizeX, char message[]);
int save();
int drawrows();
int calculatearraypos(long row, long index);
int calculatedisplaylength(long row);
int calculatedisplaypos(long row, long arrayindex);

#endif
