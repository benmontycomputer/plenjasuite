#ifndef PINT_H_
#define PINT_H_

int getsize(int *x, int *y);
void writespecificline(long line, char *chars);
int edit(char filename1[]);
void writespecificlinewithcolor(long line, char *chars, int textcolor, int backgroundcolor);
//int countlines(char *filename);

int insertrow(long insertbeforeindex, char row[], long rowlen);
int insertchar(long row, long insertbeforeindex, char *insertchar);
int expandrow(long row);
int expandrows();
int drawlinenocopy(long row, long offset);
int drawline(long row, char *line, long offset, long linelen);
void writespecificcharswithcolor(long line, long charindex, char *chars, int textcolor, int backgroundcolor);
int removerow(long index);
int deletechar(long row, long deleteindex);

int calculateoffset(long row, bool movingright);
int calculatetabwidth(long row, long index);//, bool realpos);
int processchar(int sizeY, int sizeX, long charin);
int stdmessage(int sizeY, int sizeX, char *message);
int save();
int drawrows();
int calculatearraypos(long row, long index);
int calculatedisplaylength(long row);
int calculatedisplaypos(long row, long arrayindex);

#endif
