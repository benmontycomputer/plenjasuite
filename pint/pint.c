#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<ncurses.h>
#include<ctype.h>

#define TMPREFIX " Pint Editor - "

int getsize(int *x, int *y);
void writespecificline(int line, char *chars[]);
int edit(char filename1[]);
void writespecificlinewithcolor(int line, char *chars[], int textcolor, int backgroundcolor);
int countlines(char *filename);

int insertrow(int insertbeforeindex, char row[]);
int insertchar(int row, int insertbeforeindex, char insertchar[]);
int expandrow(int row);
int expandrows();
int drawlinenocopy(int row);
int drawline(int row, char *line, int offset);
void writespecificcharswithcolor(int line, int charindex, char *chars[], int textcolor, int backgroundcolor);
int removerow(int index);
int deletechar(int row, int deleteindex);
char* insert_char_realloc (char *str, int len, char c, int pos);
char* insert_char_malloc (char *str, int len, char c, int pos);

typedef struct editorRow
{
    //char *chars;
    char *chars;
} editorRow;

static struct editorRow *rows;
static int numberOfRows = 0;
static int actualX = 0;
static int rowOffset = 0;
//static char rows[4][128];
//static char * rows;

int main(int argc, char **argv) {
    initscr();
    
    if (argc == 2) {
        writespecificline(10, "1 argument passed");
        //char filename = malloc(sizeof(argv[1]));
        //strcpy(&argv[1], &filename);
        //printf("%s", &argv[1]);
        int returnval = edit(argv[1]);
        endwin();
        return returnval;
    }
    
    writespecificline(0, "Welcome to the Pint text editor.");
    
    int sizeX = 0;
    int sizeY = 0;
    
    char sizechar[16];
    
    getsize(&sizeX, &sizeY);
    
    sprintf(sizechar, "%d %s %d", sizeX, "x", sizeY);
    
    writespecificline(2, "terminal size:");
    writespecificline(3, sizechar);
    writespecificline(5, "press enter to continue...");
    
    refresh();
    
    getch();
    
    endwin();
}

int edit(char filename1[]) {
	if (start_color() != OK)
	{
		printf("Couldn't start color.");
		return 2;
	}
	
	clear();
	noecho();
	cbreak();
	
    int sizeX = 0;
    int sizeY = 0;
    
    getsize(&sizeX, &sizeY);
    
    //printf("%d", sizeY);
    
    FILE * file = fopen(filename1, "r");
    if (file == NULL)
    {
        printf("no file...");
        return 1;
    }
    
    char* currentline;
    ssize_t currentread;
    size_t len = 0;
    
    int linenumber = 0;
    
    rows = malloc(countlines(filename1) * sizeof(editorRow));
    //printf(" %d ", countlines(filename1));
    
    while ((currentread = getline(&currentline, &len, file)) != -1)
    {
        rows[linenumber].chars = malloc(strlen(currentline));
        //currentline = realloc(currentline, strlen(currentline) + 1);
        //strncat(currentline, '\0', 1);
        //char * currentline2 = malloc(strlen(currentline) + 1);
        //strcpy(currentline2, currentline);
        //currentline2[strlen(currentline2) - 1] = '\0';
        //currentline[strlen(currentline) - 1] = '\0';
        //strcpy(rows[linenumber].chars, currentline);
        insertrow(linenumber, currentline);
        //free(currentline2);
        linenumber = linenumber + 1;
        //numberOfRows++;
    }
    fclose(file);
    free(currentline);
    
    //insertrow(linenumber, "");
    
    //insertrow(11, "new w 3");
    //removerow(11);
    //insertchar(2, 0, 'n');
    //insertchar(2, 5, 'r');
    //insertchar(2, 6, 'o');
    
    int i = 0;
    
    while (i + 3 < sizeY, i < numberOfRows)
    {
        //writespecificline(i + 1, rows[i].chars);
        //insertrow(i, rows[i].chars);
        drawlinenocopy(i);
        i = i + 1;
    }
    
    char *topmessage;
    
    topmessage = malloc(strlen(TMPREFIX) + strlen(filename1));
    
    strcpy(topmessage, TMPREFIX);
    strcat(topmessage, filename1);
    
    writespecificlinewithcolor(0, topmessage, COLOR_BLACK, COLOR_WHITE);
    free(topmessage);
    
    //intrflush(stdscr, false);
    
    keypad(stdscr, true);
    
    //halfdelay(4);
    
    move(1,0);
    refresh();
    
    while (true)
    {
    	int ch = getchar();
		int charin = ch;
		if (charin == 27)
		{
			int ch2 = getchar();
			int ch3 = getchar();
			charin = (ch * 10000) + (ch2 * 100) + ch3;
			//printf("%d", charin);
		}
		
    	processchar(sizeY, sizeX, charin);
    	//printf("%d ", ch);
    	//getch();
    }
    
    free(rows);
}

int processchar(int sizeY, int sizeX, int charin)
{
	/*int ch = getchar();
	int charin = ch;
	if (charin == 27)
	{
		int ch2 = getchar();
		int ch3 = getchar();
		charin = (ch * 10000) + (ch2 * 100) + ch3;
		//printf("%d", charin);
	}*/
	//27 79 68 is left arrow, 27 79 67 is right arrow,
	//27 79 66 is down arrow, 27 79 65 is up arrow
	if (charin == 277968) // left arrow
	{
		int y,x;
		getyx(stdscr, y, x);
		
		if (actualX == 0)
		{
			if (y == 1)
			{
				// can't move
			}
			else
			{
				int lentmp = strlen(rows[y - 2].chars);
				if (lentmp >= sizeX)
				{
					move(y - 1, sizeX - 1);
				}
				else
				{
					move(y - 1, strlen(rows[y - 2].chars));
				}
				actualX = lentmp;
				drawlinenocopy(y - 2); //update row
				// later, make it so scrolling happens here if needed
			}
		}
		else
		{
			move(y, x - 1);
			actualX--;
		}
		drawlinenocopy(y - 1); //update the row
		refresh();
	}
	else if (charin == 277967) // right arrow
	{
		refresh();
		int y,x;
		getyx(stdscr, y, x);
		
		if (actualX == strlen(rows[y - 1].chars))
		{
			if (y == numberOfRows)
			{
				// can't move
			}
			else
			{
				move(y + 1, 0);
				actualX = 0;
				drawlinenocopy(y - 1); //update the row being left
				// later, make it so scrolling happens here if needed
			}
		}
		else
		{
			if (x + 1 < sizeX)
			{
				move(y, x + 1);
			}
			//else
			actualX++;
			drawlinenocopy(y - 1);
		}
		
		drawlinenocopy(y - 1);
		refresh();
		// update the row for scrolling
	}
	else if (charin == 277966)
	{}
	else if (charin == 277965)
	{}
	else if (isalnum(charin) || charin == ' ')
	{
		//printf("%d", charin);
		//printf("isalnum");
		int y,x;
		getyx(stdscr, y, x);
		char *newchar = malloc(1);
		newchar = charin;
		insertchar(y - 1, x, newchar);
		move(y, x + 1);
		actualX++;
		
		drawlinenocopy(y - 1);
		//fprintf(stderr, "%s \n", rows[y - 1].chars);
		
		refresh();
	}
	else if (charin == 8 || charin == 127) // backspace
	{
		int y,x;
		getyx(stdscr, y, x);
		if (actualX == 0 && x == 0)
		{
			if (y == numberOfRows)
			{
				insertrow(y, "");
			}
			
			int nextActualX = strlen(rows[y - 1].chars);
			
			char *tmp;
			
			tmp = malloc(strlen(rows[y - 2].chars) + strlen(rows[y - 1].chars) + 1);
			
			strcpy(tmp, rows[y - 2].chars);
			strcat(tmp, rows[y - 1].chars);
			
			rows[y - 2].chars = malloc(strlen(tmp));
			strcpy(rows[y - 2].chars, tmp);
			
			removerow(y - 1);
			
			int i = 0;
			int actualXCache = actualX;
			actualX = 0;
			while (i < numberOfRows)
			{
				move(i + 1, 0);
				if (i == y - 2)
				{
					actualX = actualXCache;
					drawlinenocopy(i);
					actualX = 0;
				}
				else
				{
					drawlinenocopy(i);
				}
				
				i++;
			}
			
			move(numberOfRows + 1, 0);
			clrtoeol();
			
			move(y, 0);
			
			//actualX = actualXCache;
			actualX = 0;
			i = 0;
			while (i <= nextActualX)
			{
				processchar(sizeY, sizeX, 277968);
				i++;
			}
			
			removerow(y);
			//actualX = nextActualX;
		}
		else
		{
			int y,x;
			getyx(stdscr, y, x);
			deletechar(y - 1, x);
			drawlinenocopy(y - 1);
			processchar(sizeY, sizeX, 277968);
		}
		// move cursor
		//processchar(sizeY, sizeX, 277968);
		refresh();
	}
}

int drawlinenocopy(int row)
{
	int y,x;
	getyx(stdscr, y, x);
	char *text = malloc(strlen(rows[row].chars));
    strcpy(text, rows[row].chars);
    drawline(row, text, actualX - x);
    free(text);
}

int drawline(int row, char *line, int offset)
{
	rows[row].chars = malloc(strlen(line));
	strcpy(rows[row].chars, line);
	
	int sizeY, sizeX;
	getsize(&sizeX, &sizeY);
	
	if (strlen(line) < sizeX)
	{
		writespecificline(row + 1, rows[row].chars);
	}
	else
	{
		char *tmp = malloc(sizeX);
		
		strncpy(tmp, rows[row].chars+offset, sizeX);
		
		writespecificline(row + 1, tmp);
		
		if (offset > 0)
		{
			//tmp[0] = '<';
			writespecificcharswithcolor(row + 1, 0, "<", COLOR_BLACK, COLOR_WHITE);
		}
		if (offset + sizeX < strlen(rows[row].chars))
		{
			//tmp[sizeX - 1] = '>';
			writespecificcharswithcolor(row + 1, sizeX - 1, ">", COLOR_BLACK, COLOR_WHITE);
		}
		
		free(tmp);
	}
	// implement scrolling in the future
}

//https://stackoverflow.com/questions/12733105/c-function-that-counts-lines-in-file
int countlines(char *filename)
{
  // count the number of lines in the file called filename                                    
  FILE *fp = fopen(filename,"r");
  int ch=0;
  int lines=0;

  if (fp == NULL);
  return 0;

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

int insertrow(int insertbeforeindex, char row[])//, char * rows2)
{
	expandrows();
	
	int i = numberOfRows - 1;
	//int oldlen = strlen(rows2);
	
	//rows[numberOfRows - 1].chars = malloc(strlen(row));
	
	while (i >= insertbeforeindex + 1)
	{
		//printf("%s %d", "i=", i);
		rows[i].chars = malloc(strlen(rows[i - 1].chars));
		memcpy(rows[i].chars, rows[i - 1].chars, strlen(rows[i - 1].chars));
		
		i--;
	}
	
	rows[insertbeforeindex].chars = malloc(strlen(row));
	memcpy(rows[insertbeforeindex].chars, row, strlen(row));
}
int expandrows()
{
	//struct editorRow * tmp = &rows;
	//tmp = realloc(tmp, (numberOfRows + 1) * sizeof(editorRow));
	
	editorRow *tmp;
	
	tmp = malloc(numberOfRows * sizeof(editorRow));
	
	int i = 0;
	
	while (i < numberOfRows)
	{
		//printf("%s", rows[i].chars);
		tmp[i].chars = malloc(sizeof(editorRow));
		tmp[i].chars = malloc(strlen(rows[i].chars));
		strncpy(tmp[i].chars, rows[i].chars, strlen(rows[i].chars));
		//printf("%s", tmp[i].chars);
		i++;
	}
	
	//rows = realloc(rows, (numberOfRows + 1) * sizeof(editorRow));
	rows = malloc((numberOfRows + 1) * sizeof(editorRow));
	
	i = 0;
	
	while (i < numberOfRows)
	{
		//printf("%s", tmp[i].chars);
		rows[i].chars = malloc(strlen(tmp[i].chars));
		strncpy(rows[i].chars, tmp[i].chars, strlen(tmp[i].chars));
		//printf("%s", rows[i].chars);
		i++;
	}
	
	//rows = tmp;
	
	numberOfRows++;
}

int removerow(int index)
{
	//struct editorRow * tmp = &rows;
	//tmp = realloc(tmp, (numberOfRows + 1) * sizeof(editorRow));
	
	editorRow tmp[numberOfRows];
	
	int i = 0;
	
	while (i < index)
	{
		//printf("%s", rows[i].chars);
		tmp[i].chars = malloc(strlen(rows[i].chars));
		strncpy(tmp[i].chars, rows[i].chars, strlen(rows[i].chars));
		//printf("%s", tmp[i].chars);
		i++;
	}
	i++;
	while (i < numberOfRows)
	{
		//printf("%s", rows[i].chars);
		tmp[i - 1].chars = malloc(strlen(rows[i].chars));
		strncpy(tmp[i - 1].chars, rows[i].chars, strlen(rows[i].chars));
		//printf("%s", tmp[i].chars);
		i++;
	}
	
	rows = malloc((numberOfRows - 1) * sizeof(editorRow));
	
	i = 0;
	
	while (i < numberOfRows - 1)
	{
		//printf("%s", tmp[i].chars);
		rows[i].chars = malloc(strlen(tmp[i].chars));
		strncpy(rows[i].chars, tmp[i].chars, strlen(tmp[i].chars));
		//printf("%s", rows[i].chars);
		i++;
	}
	
	//rows = tmp;
	
	numberOfRows--;
	
	//free(tmp);
}

int insertchar(int row, int insertbeforeindex, char insertchar[])//, char * rows2)
{
	//expandrow(row);
    
    /*//https://stackoverflow.com/questions/4761764/how-to-remove-first-three-characters-from-string-with-c
    
    if (insertbeforeindex == 0)
    {
    	//char *buf = malloc(strlen(rows[row].chars) + 1);
    	char buf[strlen(rows[row].chars)];
    	
		strncpy(buf, &insertchar, strlen(rows[row].chars)); // copy all of insert[] at the end
		strcat(buf, rows[row].chars); // copy the rest
		//buf[strlen(buf) - 1] = '\0';
		rows[row].chars = malloc(strlen(buf));
		strncpy(rows[row].chars, buf, strlen(buf)); 
		//free(buf);
    }
    else
    {
		//char *buf = malloc(strlen(rows[row].chars) + 1);
		char buf[strlen(rows[row].chars)];
		//memset(buf, 0, 100);
		
		strncpy(buf, rows[row].chars, insertbeforeindex); // copy at most first pos characters
		//strcat(buf, &insertchar); // copy all of insert[] at the end
		fprintf(stderr, "current row text: %s \n", rows[row].chars);
		fprintf(stderr, "insertchar: %s \n \n", &insertchar);
		buf[insertbeforeindex] = insertchar;
		//if (strlen(rows[row].chars) - 1 <= insertbeforeindex)
		if (false)
		{
			
		}
		else
		{
			char * rest = malloc(strlen(rows[row].chars+insertbeforeindex));
			strcpy(rest, rows[row].chars+insertbeforeindex);
			buf[insertbeforeindex + 1] = NULL;
			strcat(buf, rest); // copy the rest
			free(rest);
		}
		//buf[strlen(buf) - 1] = '\0';
		rows[row].chars = realloc(rows[row].chars, strlen(buf));
		strncpy(rows[row].chars, buf, strlen(buf));   // copy it back to subject
		// Note that subject[] must be big enough, or else segfault.
		// deallocate buf[] here, if used malloc()
		// e.g. free(buf);
		//free(buf);
    }*/
    /*const char *source = rows[row].chars;

	// find length of the source string
	int source_len = strlen(source);

	// find length of the new string
	int result_len = source_len + 2; // 2 quotation marks

	// allocate memory for the new string (including null-terminator) 
	char *result = malloc((result_len + 1) * sizeof(char));

	// write and verify the string 
	if (sprintf(result, "\"%s\"", source) != result_len) { /* handle error *//* }
	rows[row].chars = result;
	free(source);*/
	//char *tmp = malloc(strlen(rows[row].chars));
	//strcpy(tmp, rows[row].chars);
	//insert_char_realloc(tmp, strlen(tmp), 'd', insertbeforeindex);
	//rows[row].chars = malloc(strlen(tmp));
	//strcpy(rows[row].chars, tmp);
	//free(tmp);
	char *tmp = insert_char_malloc(rows[row].chars, strlen(rows[row].chars), 'd', insertbeforeindex);
	rows[row].chars = malloc(strlen(tmp));
	memmove(rows[row].chars, tmp, strlen(tmp));
}

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

char* insert_char_realloc (char *str, int len, char c, int pos)
{
int i;

str = realloc(str, len + 2);
for (i = len + 1 ; i > pos ; --i)
str[i] = str[i - 1];
str[i] = c;
return str;
}

int deletechar(int row, int deleteindex)
{
	deleteindex--;
	memmove(&rows[row].chars[deleteindex], &rows[row].chars[deleteindex + 1], strlen(rows[row].chars) - deleteindex);
	//rows[row].chars = realloc(rows[row].chars, strlen(rows[row].chars) - 1);
	char *tmp = malloc(strlen(rows[row].chars) - 1);
	memcpy(tmp, rows[row].chars, strlen(rows[row].chars) - 1);
	//rows[row].chars = malloc(strlen(tmp));
	//memcpy(rows[row].chars, tmp, strlen(tmp));
	free(rows[row].chars);
	rows[row].chars = malloc(strlen(tmp));
	//rows[row].chars = tmp;
	memcpy(rows[row].chars, tmp, strlen(tmp));
	free(tmp);
	//free(tmp);
	/*char *tmp = malloc(strlen(rows[row].chars));
	// = rows[row].chars;
	strcpy(rows[row].chars, tmp);
	//free(rows[row].chars);
	rows[row].chars = malloc(strlen(rows[row].chars) - 1);
	strncpy(rows[row].chars, tmp, deleteindex);
	strcat(rows[row].chars, tmp+(deleteindex + 1));
	free(tmp);*/
}

int expandrow(int row)
{
	//editorRow *newrow = malloc(sizeof(editorRow));
	char *newrow;
	//newrow = malloc(strlen(rows[row].chars) + 1);
	
	fprintf(stderr, "Row before expansion: %s\n", rows[row].chars);
	
	newrow = malloc((strlen(rows[row].chars)) + 1);
	
	strncpy(newrow, rows[row].chars, strlen(rows[row].chars));
	
	free(rows[row].chars);
	//rows[row].chars = realloc(rows[row].chars, strlen(newrow));
	rows[row].chars = newrow;
	//rows[row].chars = malloc(strlen(newrow) + 1);
	//rows[row] = newrow;
	//newrow[strlen(newrow) - 1] = '\0';
	//strncpy(rows[row].chars, newrow, strlen(newrow));
	//strcat(rows[row].chars, ''
	
	//free(newrow);
}

int getsize(int *x, int *y) {
    getmaxyx(stdscr, *y, *x);
    return 0;
}

void writespecificline(int line, char *chars[]) {
	int y,x;
	getyx(stdscr, y, x);
    move(line, 0);
    clrtoeol();
    addstr(chars);
    move(y, x);
}
void writespecificlinewithcolor(int line, char *chars[], int textcolor, int backgroundcolor) {
	int y,x;
	getyx(stdscr, y, x);
	init_pair(1, textcolor, backgroundcolor);
	attrset(COLOR_PAIR(1));
    move(line, 0);
    clrtoeol();
    addstr(chars);
    attroff(COLOR_PAIR(1));
    move(y, x);
}
void writespecificcharswithcolor(int line, int charindex, char *chars[], int textcolor, int backgroundcolor) {
	int y,x;
	getyx(stdscr, y, x);
	init_pair(1, textcolor, backgroundcolor);
	attrset(COLOR_PAIR(1));
    move(line, charindex);
    addstr(chars);
    attroff(COLOR_PAIR(1));
    move(y, x);
}
