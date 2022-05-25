#ifndef PINT_H_
#define PINT_H_

int getsize(int *x, int *y);
void writespecificline(int line, char *chars);
int edit(char filename1[]);
void writespecificlinewithcolor(int line, char *chars, int textcolor, int backgroundcolor);
int countlines(char *filename);

int insertrow(int insertbeforeindex, char row[]);
int insertchar(int row, int insertbeforeindex, char insertchar[]);
int expandrow(int row);
int expandrows();
int drawlinenocopy(int row, int offset);
int drawline(int row, char *line, int offset, int linelen);
void writespecificcharswithcolor(int line, int charindex, char *chars, int textcolor, int backgroundcolor);
int removerow(int index);
int deletechar(int row, int deleteindex);
char* insert_char_realloc (char *str, int len, char c, int pos);
char* insert_char_malloc (char *str, int len, char c, int pos);

int calculateoffset(int row, bool movingright);
int calculatetabwidth(int row, int index);//, bool realpos);

#endif