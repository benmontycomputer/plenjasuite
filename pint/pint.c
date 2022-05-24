#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<ncurses.h>
#include<ctype.h>
#include<signal.h>
#include<errno.h>

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
int drawlinenocopy(int row, int offset);
int drawline(int row, char *line, int offset);
void writespecificcharswithcolor(int line, int charindex, char *chars[], int textcolor, int backgroundcolor);
int removerow(int index);
int deletechar(int row, int deleteindex);
char* insert_char_realloc (char *str, int len, char c, int pos);
char* insert_char_malloc (char *str, int len, char c, int pos);

int calculateoffset(int row, bool movingright);

typedef struct editorRow
{
    //char *chars;
    char *chars;
    int length;
} editorRow;

static struct editorRow *rows;
static int numberOfRows = 0;
static int actualX = 0;
static int xRequest = 0;
static int rowOffset = 0;
static int verticalOffset = 0;

//https://stackoverflow.com/questions/7978315/ctrl-c-eaten-by-getchar
static char done = 0;
static void sigHandler(int signum)
{
    done = 1;
}

static char *filename;

//static char rows[4][128];
//static char * rows;

int main(int argc, char **argv) {
    //initscr();
    
    if (argc == 2) {
    	if (strcmp(argv[1], "--help") == 0)
    	{
    		printf("Usage:\n");
    		printf(argv[0]);
    		printf(" [FILE]\n");
    		return 0;
    	}
    	if (strcmp(argv[1], "--version") == 0)
    	{
    		printf("pint v0.0.1-alpha.1\n");
    		return 0;
    	}
    	
    	initscr();
    	
        writespecificline(10, "1 argument passed");
        //char filename = malloc(sizeof(argv[1]));
        //strcpy(&argv[1], &filename);
        //printf("%s", &argv[1]);
        int returnval = edit(argv[1]);
        endwin();
        return returnval;
    }
    
    initscr();
    
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
	
	filename = malloc(strlen(filename1));
	strcpy(filename, filename1);
	
	clear();
	noecho();
	cbreak();
	
    int sizeX = 0;
    int sizeY = 0;
    
    //https://stackoverflow.com/questions/7978315/ctrl-c-eaten-by-getchar
    struct sigaction sa;
    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_handler = sigHandler;
    sa.sa_flags = 0;// not SA_RESTART!;

    //sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    
    getsize(&sizeX, &sizeY);
    
    //printf("%d", sizeY);
    
    FILE * file = fopen(filename1, "r");
    if (file == NULL)
    {
        printf("no file...");
        return 1;
    }
    
    char* currentline;
    size_t currentread;
    size_t len = 0;
    
    int linenumber = 0;
    
    rows = malloc(countlines(filename1) * sizeof(editorRow));
    //printf(" %d ", countlines(filename1));
    
    while ((currentread = getline(&currentline, &len, file)) != -1)
    {
        //rows[linenumber].chars = malloc(strlen(currentline));
        //currentline = realloc(currentline, strlen(currentline) + 1);
        //strncat(currentline, '\0', 1);
        //char * currentline2 = malloc(strlen(currentline) + 1);
        //strcpy(currentline2, currentline);
        //currentline2[strlen(currentline2) - 1] = '\0';
        //currentline[strlen(currentline) - 1] = '\0';
        //strcpy(rows[linenumber].chars, currentline);
        currentline[strlen(currentline) - 1] = '\0';
        insertrow(linenumber, currentline);
        //free(currentline2);
        linenumber = linenumber + 1;
        //numberOfRows++;
    }
    fclose(file);
    free(currentline);
    //return;
    
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
        drawlinenocopy(i, 0);
        i = i + 1;
    }
    
    char *topmessage = malloc(strlen(TMPREFIX) + strlen(filename1));
    
    strcpy(topmessage, TMPREFIX);
    strcat(topmessage, filename1);
    
    writespecificlinewithcolor(0, topmessage, COLOR_BLACK, COLOR_WHITE);
    free(topmessage);
    
    char *bottom1 = malloc(sizeX);
    
    memcpy(bottom1, "Ctrl+X: Save", 12);
    i = 12;
    while (i < sizeX)
    {
    	//strncat(bottom1, , 1);
    	bottom1[i] = ' ';
    	i++;
    }
    /*char *bottom2 = malloc(sizeX);
    memcpy(bottom2, " ", 2);
    i = 0;
    while (i < sizeX)
    {
    	strncat(bottom2, " ", 1);
    	i++;
    }*/
    
    writespecificlinewithcolor(sizeY - 2, bottom1, COLOR_BLACK, COLOR_WHITE);
    //writespecificlinewithcolor(sizeY - 1, bottom2, COLOR_BLACK, COLOR_WHITE);
    free(bottom1);
    //free(bottom2);
    
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
		
    	int result = processchar(sizeY, sizeX, charin);
    	if (result == 1)
    	{
    		save();
    		return;
    	}
    	//printf("%d ", ch);
    	//getch();
    }
    /*while (true)
    {
    	printf("%d ", getchar());
    }*/
    
    //free(rows);
}

int save()
{
	char filename1[strlen(filename)];
	strcpy(filename1, filename);
	
	FILE *file = fopen(filename1, "w");
	if (file)
	{
		int i = 0;
		while (i < numberOfRows)
		{
			char tmp[rows[i].length];
			strcpy(tmp, rows[i].chars);
			fputs(rows[i].chars, file);
			fputs("\n", file);
			
			//printf("adding line");
			
			i++;
		}
		
		//file.close();
		fclose(file);
		
		//endwin();
	}
	else
	{
		fprintf(stderr, "%s %s", "failed to save: ", filename1);
	}
	
	//free(file);
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
				if (verticalOffset == 0)
				{
					// can't move
				}
				else
				{
					verticalOffset--;
					drawrows();
				}
			}
			else
			{
				int lentmp = rows[y + verticalOffset - 2].length;//strlen(rows[y - 2].chars);
				/*if (lentmp >= sizeX)
				{
					move(y - 1, sizeX - 1);
				}
				else
				{
					move(y - 1, lentmp);//strlen(rows[y - 2].chars));
				}*/
				actualX = lentmp;
				xRequest = actualX;
				rowOffset = lentmp - sizeX + 8;
				rowOffset = calculateoffset(y + verticalOffset - 2, false);
				if (lentmp >= sizeX)
				{
					move(y - 1, actualX - rowOffset);
				}
				else
				{
					move(y - 1, lentmp);//strlen(rows[y - 2].chars));
				}
				drawlinenocopy(y + verticalOffset - 2, rowOffset); //update row
				// later, make it so scrolling happens here if needed
				drawlinenocopy(y + verticalOffset - 1, 0);
			}
		}
		else
		{
			//move(y, x - 1);
			actualX--;
			xRequest = actualX;
			//fprintf(stderr, "%d ", rowOffset);
			rowOffset = calculateoffset(y - 1 + verticalOffset, false);
			//fprintf(stderr, "%d \n", rowOffset);
			drawlinenocopy(y - 1 + verticalOffset, rowOffset);
			move(y, actualX - rowOffset);
		}
		//drawlinenocopy(y - 1, calculateoffset(y - 1, false)); //update the row
		
		if (y + verticalOffset < numberOfRows)
		{
			int actualXCache = actualX;
			actualX = 0;
			int xCache, yCache;
			getyx(stdscr, yCache, xCache);
			move(y, 0);
			drawlinenocopy(y + verticalOffset, 0);
			move(yCache, xCache);
			actualX = actualXCache;
		}
		refresh();
	}
	else if (charin == 277967) // right arrow
	{
		refresh();
		int y,x;
		getyx(stdscr, y, x);
		
		if (actualX == rows[y - 1 + verticalOffset].length)//strlen(rows[y - 1].chars))
		{
			if (y + verticalOffset >= numberOfRows || y > sizeY - 2)
			{
				if (y + verticalOffset >= numberOfRows)
				{
					// cant move
				}
				else
				{
					verticalOffset++;
					drawrows();
					move(y, 0);
					actualX = 0;
					xRequest = actualX;
					rowOffset = 0;
					drawlinenocopy(y - 1 + verticalOffset, 0);
				}
			}
			else
			{
				move(y + 1, 0);
				actualX = 0;
				xRequest = actualX;
				rowOffset = 0;
				drawlinenocopy(y - 1 + verticalOffset, 0); //update the row being left
				// later, make it so scrolling happens here if needed
			}
		}
		else
		{
			//if (x + 1 < sizeX)
			//{
			//	move(y, x + 1);
			//}
			//else
			//move(y, calculateoffset(y - 1));
			actualX++;
			rowOffset = calculateoffset(y - 1 + verticalOffset, true);
			move(y, actualX - rowOffset);
			xRequest = actualX;
			drawlinenocopy(y - 1 + verticalOffset, rowOffset);
		}
		
		//drawlinenocopy(y - 1, calculateoffset(y - 1, true));
		if (y + verticalOffset < numberOfRows)
		{
			int actualXCache = actualX;
			actualX = 0;
			int xCache, yCache;
			getyx(stdscr, yCache, xCache);
			move(y, 0);
			drawlinenocopy(y + verticalOffset, 0);
			move(yCache, xCache);
			actualX = actualXCache;
		}
		refresh();
		// update the row for scrolling
	}
	else if (charin == 277966) // down arrow
	{
		int y,x;
		getyx(stdscr, y, x);
		int tmpint = rows[y - 1 + verticalOffset].length - actualX + 1;
		
		//if (y + verticalOffset >= numberOfRows)
		//{
		//	return 0;
		//}
		
		int xRequestCache = xRequest;
		if (xRequest > rows[y + verticalOffset].length)
		{
			tmpint += rows[y + verticalOffset].length;
		}
		else
		{
			tmpint += xRequest;
		}
		
		while (tmpint > 0)
		{
			processchar(sizeY, sizeX, 277967);
			tmpint--;
		}
		xRequest = xRequestCache;
	}
	else if (charin == 277965) // up arrow
	{
		/*int y,x;
		getyx(stdscr, y, x);
		int tmpint = actualX + 2;
		while (tmpint > 0)
		{
			processchar(sizeY, sizeX, 277968);
			tmpint--;
		}*/
		
		int y,x;
		getyx(stdscr, y, x);
		int tmpint = actualX + 1; // -1?
		
		int xRequestCache = xRequest;
		
		if (y - 2 + verticalOffset < 0)
		{
			return;
		}
		
		if (xRequest < rows[y - 2 + verticalOffset].length)
		{
			//tmpint += rows[y - 2].length;
			tmpint += rows[y - 2 + verticalOffset].length - xRequest;
		}
		else
		{
			//tmpint += rows[y - 2].length - xRequest;
		}
		
		while (tmpint > 0)
		{
			processchar(sizeY, sizeX, 277968);
			tmpint--;
		}
		
		if (rowOffset < sizeX)
		{
			rowOffset = 0;
			//rowOffset = calculateoffset(y, true);
			drawlinenocopy(y - 2 + verticalOffset, 0);//actualX - rowOffset);
		}
		
		xRequest = xRequestCache;
	}
	else if (charin == 9)
	{
		int i = 4;
		while(i > 0)
		{
			processchar(sizeY, sizeX, ' ');
			i--;
		}
	}
	else if (charin == 13) // enter (carriage return)
	{
		//fprintf(stderr, "enter");
		int y,x;
		getyx(stdscr, y, x);
		
		insertrow(y, "");
		int i = y - 1;
		int actualXCache = actualX;
		actualX = 0;
		int xCache, yCache;
		getyx(stdscr, yCache, xCache);
		move(y, 0);
		
		while (i < numberOfRows && i + 2 < sizeY)
		{
			drawlinenocopy(i + verticalOffset, 0);
			
			i++;
		}
		
		move(yCache, xCache);
		actualX = actualXCache;
		
		refresh();
	}
	else if (charin == 24) // ctrl+x?
	{
		//fprintf(stderr, "ctrl+x");
		//saveandclose();
		return 1;
	}
	else if (isalnum(charin) || charin == ' ' || charin == '!' || charin == '@' || charin == '#' || charin == '$' || charin == '%' || charin == '^' || charin == '&' || charin == '*' || charin == '(' || charin == ')' || charin == '-' || charin == '_' || charin == '=' || charin == '+' || charin == '`' || charin == '~' || charin == '[' || charin == ']' || charin == '{' || charin == '}' || charin == '\\' || charin == '|' || charin == '\'' || charin == '"' || charin == ';' || charin == ':' || charin == ',' || charin == '.' || charin == '<' || charin == '>' || charin == '/' || charin == '?')
	{
		//printf("%d", charin);
		//printf("isalnum");
		int y,x;
		getyx(stdscr, y, x);
		char *newchar = charin;//malloc(1);
		//newchar = charin;
		insertchar(y - 1 + verticalOffset, x, newchar);
		//free(newchar);
		processchar(sizeY, sizeX, 277967);
		//move(y, x + 1);
		//actualX++;
		//xRequest = actualX;
		
		//drawlinenocopy(y - 1, calculateoffset(y - 1));
		if (y + verticalOffset < numberOfRows)
		{
			int actualXCache = actualX;
			actualX = 0;
			int xCache, yCache;
			getyx(stdscr, yCache, xCache);
			move(y, 0);
			drawlinenocopy(y + verticalOffset, 0);
			move(yCache, xCache);
			actualX = actualXCache;
		}
		//fprintf(stderr, "%s \n", rows[y - 1].chars);
		
		refresh();
	}
	else if (charin == 8 || charin == 127) // backspace
	{
		int y,x;
		getyx(stdscr, y, x);
		if (actualX == 0 && x == 0)
		{
			if (y == 1)
			{
				return;
			}
			if (y == numberOfRows)
			{
				insertrow(y, "");
			}
			
			int nextActualX = rows[y - 1 + verticalOffset].length; //strlen(rows[y - 1].chars);
			
			char *tmp;
			
			tmp = malloc(rows[y - 2 + verticalOffset].length + rows[y - 1 + verticalOffset].length + 1); //malloc(strlen(rows[y - 2].chars) + strlen(rows[y - 1].chars) + 1);
			
			strcpy(tmp, rows[y - 2 + verticalOffset].chars);
			strcat(tmp, rows[y - 1 + verticalOffset].chars);
			
			rows[y - 2 + verticalOffset].chars = malloc(strlen(tmp));
			strcpy(rows[y - 2 + verticalOffset].chars, tmp);
			
			removerow(y - 1 + verticalOffset);
			
			int i = 0;
			int actualXCache = actualX;
			actualX = 0;
			while (i < numberOfRows)
			{
				move(i + 1, 0);
				if (i == y - 2 + verticalOffset)
				{
					actualX = actualXCache;
					drawlinenocopy(i + verticalOffset, 0);//calculateoffset(y - 2, false));
					actualX = 0;
				}
				else
				{
					drawlinenocopy(i + verticalOffset, 0);
				}
				
				i++;
			}
			
			move(numberOfRows + 1 - verticalOffset, 0);
			clrtoeol();
			
			move(y, 0);
			
			//actualX = actualXCache;
			actualX = 0;
			xRequest = actualX;
			i = 0;
			while (i <= nextActualX)
			{
				processchar(sizeY, sizeX, 277968);
				i++;
			}
			
			removerow(y + verticalOffset);
			//actualX = nextActualX;
		}
		else
		{
			int y,x;
			getyx(stdscr, y, x);
			deletechar(y - 1 + verticalOffset, x);
			//drawlinenocopy(y - 1, calculateoffset(y - 1, false));
			processchar(sizeY, sizeX, 277968);
		}
		// move cursor
		//processchar(sizeY, sizeX, 277968);
		if (y + verticalOffset < numberOfRows)
		{
			drawlinenocopy(y + verticalOffset, 0);//calculateoffset(y, false));
		}
		refresh();
	}
	
	return 0;
}

int drawrows()
{
	int sizeY, sizeX;
	getsize(&sizeX, &sizeY);
	
	int i = verticalOffset;
	
	while (i - verticalOffset + 1 < sizeY)
	{
		drawlinenocopy(i, 0);
		i++;
	}
}

int calculateoffset(int row, bool movingright)
{
	int sizeY, sizeX;
	getsize(&sizeX, &sizeY);
	
	if (rows[row].length <= sizeX)
	{
		return 0;
	}
	//else if (actualX < sizeX - 8)
	//{
	//	return 0;
	//}
	else
	{
		int i = 0;
		int y,x;
		getsize(&x, &y);
		
		/*if (actualX - x < 7)
		{
			return 8 * (x / 8);
		}*/
		
		if (movingright)
		{
			if (x < sizeX - 1)
			{
				return rowOffset;
			}
			while (i < (actualX) - (sizeX - 2))
			{
				i += 1;
			}
		}
		else
		{
			if (x > 1)
			{
				return rowOffset;
			}
			while (i < (actualX) - 2)
			{
				i += 1;
			}
		}
		
		if (rows[row].length - i < sizeX)
		{
			i = rows[row].length - sizeX + 8;
		}
		
		return i;
	}
}

int drawlinenocopy(int row, int offset)
{
	/*int y,x;
	getyx(stdscr, y, x);
	int sizeY, sizeX;
	getsize(&sizeX, &sizeY);*/
	char *text = malloc(rows[row].length);//malloc(strlen(rows[row].chars));
    strcpy(text, rows[row].chars);
    /*if (actualX < sizeX)
    {
    	drawline(row, text, 0);
    }
    else
    {
    	drawline(row, text, actualX - x);
    }*/
    drawline(row, text, offset);
    free(text);
}

int drawline(int row, char *line, int offset)
{
	rows[row].chars = malloc(strlen(line));
	strcpy(rows[row].chars, line);
	rows[row].length = strlen(line);
	
	int sizeY, sizeX;
	getsize(&sizeX, &sizeY);
	
	if (strlen(line) < sizeX)
	{
		writespecificline(row + 1 - verticalOffset, rows[row].chars);
	}
	else
	{
		char *tmp = malloc(sizeX);
		
		strncpy(tmp, rows[row].chars+offset, sizeX);
		
		writespecificline(row + 1 - verticalOffset, tmp);
		
		if (offset > 0)
		{
			//tmp[0] = '<';
			writespecificcharswithcolor(row + 1 - verticalOffset, 0, "<", COLOR_BLACK, COLOR_WHITE);
		}
		if (offset + sizeX < rows[row].length)//strlen(rows[row].chars))
		{
			//tmp[sizeX - 1] = '>';
			writespecificcharswithcolor(row + 1 - verticalOffset, sizeX - 1, ">", COLOR_BLACK, COLOR_WHITE);
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
		rows[i].chars = malloc(rows[i - 1].length);//malloc(strlen(rows[i - 1].chars));
		rows[i].length = rows[i - 1].length;
		memcpy(rows[i].chars, rows[i - 1].chars, rows[i - 1].length);//strlen(rows[i - 1].chars));
		
		i--;
	}
	
	rows[insertbeforeindex].chars = malloc(strlen(row));
	rows[insertbeforeindex].length = strlen(row);
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
		//tmp[i].chars = malloc(sizeof(editorRow));
		tmp[i].chars = malloc(rows[i].length);//malloc(strlen(rows[i].chars));
		tmp[i].length = rows[i].length;
		strncpy(tmp[i].chars, rows[i].chars, rows[i].length);//strlen(rows[i].chars));
		//printf("%s", tmp[i].chars);
		i++;
	}
	
	//rows = realloc(rows, (numberOfRows + 1) * sizeof(editorRow));
	rows = malloc((numberOfRows + 1) * sizeof(editorRow));
	
	i = 0;
	
	while (i < numberOfRows)
	{
		//printf("%s", tmp[i].chars);
		rows[i].chars = malloc(tmp[i].length);//malloc(strlen(tmp[i].chars));
		rows[i].length = tmp[i].length;
		strncpy(rows[i].chars, tmp[i].chars, tmp[i].length);//strlen(tmp[i].chars));
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
		tmp[i].chars = malloc(rows[i].length);//malloc(strlen(rows[i].chars));
		tmp[i].length = rows[i].length;
		strncpy(tmp[i].chars, rows[i].chars, rows[i].length);//strlen(rows[i].chars));
		//printf("%s", tmp[i].chars);
		i++;
	}
	i++;
	while (i < numberOfRows)
	{
		//printf("%s", rows[i].chars);
		tmp[i - 1].chars = malloc(rows[i].length);//malloc(strlen(rows[i].chars));
		tmp[i - 1].length = rows[i].length;
		strncpy(tmp[i - 1].chars, rows[i].chars, rows[i].length);//strlen(rows[i].chars));
		//printf("%s", tmp[i].chars);
		i++;
	}
	
	rows = malloc((numberOfRows - 1) * sizeof(editorRow));
	
	i = 0;
	
	while (i < numberOfRows - 1)
	{
		//printf("%s", tmp[i].chars);
		rows[i].chars = malloc(tmp[i].length);//malloc(strlen(tmp[i].chars));
		rows[i].length = tmp[i].length;
		strncpy(rows[i].chars, tmp[i].chars, tmp[i].length);//strlen(tmp[i].chars));
		//printf("%s", rows[i].chars);
		i++;
	}
	
	//rows = tmp;
	
	numberOfRows--;
	
	//free(tmp);
}

int insertchar(int row, int insertbeforeindex, char insertchar[])//, char * rows2)
{
	expandrow(row);
    
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
	char *tmp = insert_char_malloc(rows[row].chars, rows[row].length/*strlen(rows[row].chars)*/, insertchar, insertbeforeindex);
	rows[row].length = rows[row].length + 1;//strlen(tmp);
	rows[row].chars = malloc(rows[row].length);//malloc(strlen(tmp));
	memcpy(rows[row].chars, tmp, rows[row].length);
	free(tmp);
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
	memmove(&rows[row].chars[deleteindex], &rows[row].chars[deleteindex + 1], rows[row].length - deleteindex);
	//rows[row].chars = realloc(rows[row].chars, strlen(rows[row].chars) - 1);
	int newlen = rows[row].length - 1;
	char *tmp = malloc(newlen);//malloc(strlen(rows[row].chars) - 1);
	memcpy(tmp, rows[row].chars, newlen);//strlen(rows[row].chars) - 1);
	//rows[row].chars = malloc(strlen(tmp));
	//memcpy(rows[row].chars, tmp, strlen(tmp));
	free(rows[row].chars);
	rows[row].chars = malloc(newlen);
	//rows[row].chars = tmp;
	memcpy(rows[row].chars, tmp, newlen);
	rows[row].length = newlen;
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
	
	//fprintf(stderr, "Row before expansion: %s\n", rows[row].chars);
	
	int newlen = rows[row].length + 1;
	newrow = malloc(newlen);//malloc((strlen(rows[row].chars)) + 1);
	
	strncpy(newrow, rows[row].chars, newlen - 1);//strlen(rows[row].chars));
	
	free(rows[row].chars);
	//rows[row].chars = realloc(rows[row].chars, strlen(newrow));
	rows[row].chars = newrow;
	rows[row].length = newlen;
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
	//char *tmp = malloc(strlen(chars) + 1);
	//tmp[strlen(chars)] = '\0';
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
