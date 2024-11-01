#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<ncurses.h>
#include<ctype.h>
#include<signal.h>
#include<errno.h>
#include "pint.h"
#include "libplenja.h"
#include<stdint.h>

#define TMPREFIX " Pint Editor - "
#define TABSIZE 8
#define DEFAULT_FILENAME ".pinttmp"

typedef struct editorRow
{
	pstring chars;
	long displaylength;
} editorRow;

static struct editorRow *rows;
static long numberOfRows = 0;
static long actualX = 0;
static long xRequest = 0;
static long rowOffset = 0;
static long verticalOffset = 0;

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

	//char *filename;

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
			printf("pint v0.0.1-alpha.2\n");
			return 0;
		}

		filename = malloc(strlen(argv[1]));
		strcpy(filename, argv[1]);
	}
	else
	{
		filename = malloc(sizeof(DEFAULT_FILENAME));
		strcpy(filename, DEFAULT_FILENAME);
	}
	
	initscr();

	//writespecificline(10, "1 argument passed");
	//char filename = malloc(sizeof(argv[1]));
	//strcpy(&argv[1], &filename);
	//printf("%s", &argv[1]);
	int returnval = edit(filename);
	endwin();
	return returnval;
	

	/*initscr();

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

	getch();*/

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
	//sigaction(SIGTERM, &sa, NULL);

	getsize(&sizeX, &sizeY);

	//printf("%d", sizeY);

	FILE * file = fopen(filename1, "r");
	if (file == NULL)
	{
		/*file = fopen(filename1, "w+");
		if (file == NULL)
		{
			printf("Unable to open or create the file %s\n", filename1);
			return 1;
		}*/
		//printf("no file...");
		//return 1;
	}
	else
    {
        fclose(file);
    }
    
    rows = malloc(0);
    
    int numberOfRows1 = countLines(filename1);
    pstring * filelines = malloc(numberOfRows1 * sizeof(pstring));
    filelines = openFile(filename);
    
    long i = 0;
    while (i < numberOfRows1)
    {
        insertrow(i, filelines[i]);
        i++;
    }

    if (countLines(filename1) < 1)
    {
        pstring tmp;
        tmp.chars = malloc(1);
        strncpy(tmp.chars, "\0", 1);
        tmp.len = 0;
        insertrow(0, tmp);
        fprintf(stderr, "169");
    }
    
    i = 0;
    //i = 0;

    /*while (i + 3 < sizeY, i < numberOfRows)
    {
        //writespecificline(i + 1, rows[i].chars);
        //insertrow(i, rows[i].chars);
        drawlinenocopy(i, 0);
        i = i + 1;
    }*/
    drawrows();
    
    char *topmessage = malloc(strlen(TMPREFIX) + strlen(filename1));
    
    strcpy(topmessage, TMPREFIX);
    strcat(topmessage, filename1);
    
    //writespecificlinewithcolor(0, topmessage, COLOR_BLACK, COLOR_WHITE);
    free(topmessage);
    
    char *bottom1 = malloc(sizeX);
    
    strcpy(bottom1, "^X: Quit    ^W: Write");
    i = strlen(bottom1);
    while (i < sizeX)
    {
        //strncat(bottom1, , 1);
        bottom1[i] = ' ';
        i++;
    }
    /*char *bottom2 = malloc(sizeX + 1);
    memcpy(bottom2, " ", 2);
    i = 0;
    while (i < sizeX)
    {
        strncat(bottom2, " ", 1);
        i++;
    }

    bottom2[sizeX] = '\0';*/
    
    //writespecificlinewithcolor(sizeY - 2, bottom1, COLOR_WHITE, COLOR_BLACK);
    //writespecificline(sizeY - 2, bottom1);
    //writespecificlinewithcolor(sizeY - 1, bottom2, COLOR_BLACK, COLOR_WHITE);
    free(bottom1);
    writespecificcharswithcolor(sizeY - 2, 0, "^X", COLOR_BLACK, COLOR_WHITE);
    writespecificcharswithcolor(sizeY - 2, 12, "^W", COLOR_BLACK, COLOR_WHITE);
    //free(bottom2);
    
    //intrflush(stdscr, false);
    
    keypad(stdscr, true);
    
    //halfdelay(4);
    
    move(1,0);
    refresh();
    
    while (true)
    {
        int ch = getchar();
        long charin = ch;
        if (charin == 27)
        {
            // 27 91 49 59 53 67 is ctrl+up
            // 27 91 49 59 53 68 is ctrl+left
            // 27 91 49 59 53 66 is ctrl+down
            // 27 91 49 59 53 67 is ctrl+right
            int ch2 = getchar();
            int ch3 = getchar();
            charin = (ch * 10000) + (ch2 * 100) + ch3;
            if (charin == 279149)
            {
                long charinold = charin;
                charin = charinold * 100 * 100 * 100;
                int ch4 = getchar();
                int ch5 = getchar();
                int ch6 = getchar();
                charin += (ch4 * 10000) + (ch5 * 100) + ch6;
            }
            //printf("%d", charin);
        }
        
        int result = processchar(sizeY, sizeX, charin);
        if (result == 1)
        {
            //save();
            stdmessage(sizeY, sizeX, "Do you want to save? (y)es/(n)o/(c)ancel");
            refresh();
            int ch = getchar();
            if (ch == 'y')
            {
                if (save() == 0)
                {
                    return 0;
                }
                else
                {
                    stdmessage(sizeY, sizeX, "Failed to save.");
                }
            }
            else if (ch == 'n')
            {
                return 0;
            }
            else if (ch == 'c')
            {
                int y,x;
                getyx(stdscr, y, x);

                move(sizeY - 3, 0);
                clrtoeol();
                move(y, x);
                refresh();
            }
            else
            {
                int y,x;
                getyx(stdscr, y, x);

                move(sizeY - 3, 0);
                clrtoeol();
                move(y, x);
                refresh();
                
                stdmessage(sizeY, sizeX, "Invalid input. Not closing.");
                refresh();
                getchar();
            }

            
        }
        else if (result == 2)
        {
            save();
        }
        //getch();
        //printf("%d ", ch);
    }
	/*while (true)
	{
		printf("%d ", getchar());
	}*/
    
    //free(rows);
}

int save()
{
    pstring *lines = malloc(numberOfRows * sizeof(pstring));
    
    long i = 0;
    while (i < numberOfRows)
    {
        lines[i] = rows[i].chars;
        i++;
    }

    writeFile(filename, lines, numberOfRows);
    free(lines);
    //free(rowsChars);
    
    //free(file);
}

int stdmessage(int sizeY, int sizeX, char *message)
{
    int buffersize = (sizeX - strlen(message)) / 2;

    int x,y;
    getsize(&x, &y);
    move(sizeY - 3, 0);
    clrtoeol;
    move(y, x);

    if (strlen(message) + 1 < sizeX)
    {
        char *output = malloc(strlen(message) + 2);
        strncpy(output, "[", 1);
        strncat(output, message, strlen(message));
        strncat(output, "]", 1);
        writespecificcharswithcolor(sizeY - 3, buffersize - 1, output, COLOR_BLACK, COLOR_WHITE);
    }
    else
    {
        writespecificcharswithcolor(sizeY - 3, buffersize, message, COLOR_BLACK, COLOR_WHITE);
    }

    //free(message);
}

int processchar(int sizeY, int sizeX, long charin)
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
    // 27 91 49 59 53 67 is ctrl+up
    // 27 91 49 59 53 68 is ctrl+left
    // 27 91 49 59 53 66 is ctrl+down
    // 27 91 49 59 53 67 is ctrl+right
    if (charin == 279149595368) // ctrl+left
    {
        int i = 4;
        while (i > 0)
        {
            processchar(sizeY, sizeX, 277968);

            i--;
        }
    }
    else if (charin == 279149595367) // ctrl+right
    {
        int i = 4;
        while (i > 0)
        {
            processchar(sizeY, sizeX, 277967);

            i--;
        }
    }
    else if (charin == 279149595366) // ctrl+up
    {
        int i = 4;
        while (i > 0)
        {
            processchar(sizeY, sizeX, 277966);

            i--;
        }
    }
    else if (charin == 279149595365) // ctrl+down
    {
        int i = 4;
        while (i > 0)
        {
            processchar(sizeY, sizeX, 277965);

            i--;
        }
    }
    else if (charin == 277968) // left arrow
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
                    //return 0;
                }
                else
                {
                    verticalOffset--;
                    
                    actualX = rows[y + verticalOffset - 1].displaylength;
                    xRequest = actualX;
                    //rowOffset = rows[y + verticalOffset - 1].length - sizeX + 8;
                    rowOffset = calculateoffset(y + verticalOffset - 1, false);
                    drawrows();
                    move(y, actualX - rowOffset);
                    drawlinenocopy(y + verticalOffset - 1, rowOffset);
                }
            }
            else
            {
                long lentmp = rows[y + verticalOffset - 2].displaylength;//strlen(rows[y - 2].chars);
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
                if (lentmp > sizeX)
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
            long i = 1;
            long actualXOld = actualX;
            /*while (i < TABSIZE)
            {
                // fix this for < 8 width tabs later!!!
                if (rows[y - 1 + verticalOffset].chars[calculatearraypos(y - 1 + verticalOffset, actualX) - i] == '\t')
                {
                    actualX -= calculatetabwidth(y - 1 + verticalOffset, calculatearraypos(y - 1 + verticalOffset, actualX) - i, true);
                    break;
                }
                if (calculatetabwidth(y - 1 + verticalOffset, calculatearraypos(y - 1 + verticalOffset, actualX) - i, true) < i)
                {
                    break;
                }
                i++;
            }
            if (actualXOld == actualX)
            {
                actualX--;
            }*/
            if (rows[y - 1 + verticalOffset].chars.chars[calculatearraypos(y - 1 + verticalOffset, actualX) - 1] == '\t')
            {
                actualX -= calculatetabwidth(calculatedisplaypos(y - 1 + verticalOffset, calculatearraypos(y - 1 + verticalOffset, actualX) - 1));//, false);
            }
            else
            {
                actualX--;
            }
            xRequest = actualX;
            //fprintf(stderr, "%d ", rowOffset);
            //move(y, actualX - rowOffset);
            long rowOffsetOld = rowOffset;
            rowOffset = calculateoffset(y - 1 + verticalOffset, false);
            //if (rowOffsetOld == rowOffset)
            //{
            //    move(y, actualX - rowOffset);
            //}
            //fprintf(stderr, "%d \n", rowOffset);
            drawlinenocopy(y - 1 + verticalOffset, rowOffset);
            //processchar(sizeY, sizeX, 277968);
            move(y, actualX - rowOffset);
            //refresh();
        }
        //drawlinenocopy(y - 1, calculateoffset(y - 1, false)); //update the row
        
        /*if (y + verticalOffset < numberOfRows)
        {
            long actualXCache = actualX;
            actualX = 0;
            int xCache, yCache;
            getyx(stdscr, yCache, xCache);
            move(y, 0);
            drawlinenocopy(y + verticalOffset, 0);
            move(yCache, xCache);
            actualX = actualXCache;
        }*/
        refresh();
    }
    else if (charin == 277967) // right arrow
    {
        refresh();
        long y,x;
        getyx(stdscr, y, x);
        //fprintf(stderr, "display width: %d\n", rows[y - 1  + verticalOffset].displaylength);
        
        if (actualX == rows[y - 1 + verticalOffset].displaylength)//strlen(rows[y - 1].chars))
        {
            if (y + verticalOffset >= numberOfRows || y >= sizeY - 3)
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
            //    move(y, x + 1);
            //}
            //else
            //move(y, calculateoffset(y - 1));
            //int index = actualX;
            //int newlen = 0;
            /*int i = 0;
            while (i < rows[y - 1 + verticalOffset].reallength)
            {
                if (index == 0)
                {
                    i = 0;
                    break;
                }
                if (rows[y - 1 + verticalOffset].chars[i] == '\t')
                {
                    newlen += (i + 8) / 8 * 8 - i;
                }
                else
                {
                    newlen++;
                }

                i++;
                if (newlen >= index)
                {
                    if (newlen > index)
                    {
                        fprintf(stderr, "582 error");
                    }
                    break;
                }
            }*/
            long i = calculatearraypos(y - 1 + verticalOffset, actualX);
            if (rows[y - 1 + verticalOffset].chars.chars[i] == '\t')
            {
                actualX += calculatetabwidth(actualX);//, false);
            }
            else
            {
                actualX++;
            }
            rowOffset = calculateoffset(y - 1 + verticalOffset, true);
            move(y, actualX - rowOffset);
            xRequest = actualX;
            drawlinenocopy(y - 1 + verticalOffset, rowOffset);
        }
        
        //drawlinenocopy(y - 1, calculateoffset(y - 1, true));
        if (y + verticalOffset < numberOfRows)
        {
            long actualXCache = actualX;
            actualX = 0;
            int xCache, yCache;
            getyx(stdscr, yCache, xCache);
            move(y, 0);
            long lineNum = y + verticalOffset;
            //fprintf(stderr, "y: %d\n", lineNum);
            drawlinenocopy(lineNum, 0);
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
        long tmpint = rows[y - 1 + verticalOffset].chars.len - calculatearraypos(y - 1, actualX) + 1;
        
        //if (y + verticalOffset >= numberOfRows)
        //{
        //    return 0;
        //}
        
        long xRequestCache = xRequest;
        if (xRequest > rows[y + verticalOffset].displaylength)
        {
            tmpint += rows[y + verticalOffset].chars.len;
        }
        else
        {
            tmpint += calculatearraypos(y, xRequest);
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
        long tmpint = calculatearraypos(y - 1, actualX) + 1; // -1?
        
        long xRequestCache = xRequest;
        
        if (y - 2 + verticalOffset < 0)
        {
            //tmpint += 
            return 0;
        }
        else
        {
            if (xRequest < rows[y - 2 + verticalOffset].displaylength)
            {
                //tmpint += rows[y - 2].length;
                tmpint += rows[y - 2 + verticalOffset].chars.len - calculatearraypos(y - 2, xRequest);
            }
            else
            {
                //tmpint += rows[y - 2].length - xRequest;
            }
        }
        
        while (tmpint > 0)
        {
            processchar(sizeY, sizeX, 277968);
            tmpint--;
        }
        
        //if (rowOffset < sizeX)
        //{
            /*rowOffset = 0;
            //rowOffset = calculateoffset(y, true);
            if (y - 2 + verticalOffset >= 0)
            {
                drawlinenocopy(y - 2 + verticalOffset, 0);//actualX - rowOffset);
            }*/
        //    processchar(sizeY, sizeX, 277968);
        //    processchar(sizeY, sizeX, 277967);
        //}
        
        xRequest = xRequestCache;
    }
    else if (charin == 9) // tab
    {
        int y,x;
        getyx(stdscr, y, x);
        
        /*int i = 4;
        while(i > 0)
        {
            processchar(sizeY, sizeX, ' ');
            i--;
        }*/
        insertchar(y - 1 + verticalOffset, actualX, '\t');
        rowOffset = calculateoffset(y - 1 + verticalOffset, true);
        drawlinenocopy(y - 1 + verticalOffset, rowOffset);
        refresh();
    }
    else if (charin == 13) // enter (carriage return)
    {
        //fprintf(stderr, "enter");
        long y,x;
        getyx(stdscr, y, x);
        
        long arrayPos = calculatearraypos(y - 1 + verticalOffset, actualX);

        //insertrow(y + verticalOffset, rows[y - 1 + verticalOffset].chars+arrayPos, rows[y - 1 + verticalOffset].reallength - arrayPos);
        insertrow(y + verticalOffset, pstr_start_at(rows[y - 1 + verticalOffset].chars, arrayPos));

        //char remainingChars[arrayPos];// = malloc(arrayPos);//rows[y - 1].reallength - arrayPos);
        //strncpy(remainingChars, rows[y - 1 + verticalOffset].chars, arrayPos);
        pstring remainingChars = pstr_first_n(rows[y - 1 + verticalOffset].chars, arrayPos);
        removerow(y - 1 + verticalOffset);
        
        insertrow(y - 1 + verticalOffset, remainingChars);
        //rows[y - 1 + verticalOffset].displaylength = calculatedisplaylength(y - 1 + verticalOffset);

        long i = y - 1 + verticalOffset;
        long actualXCache = actualX;
        actualX = 0;
        int xCache, yCache;
        getyx(stdscr, yCache, xCache);
        move(y, 0);
        
        /*while (i < numberOfRows && i + 3 - verticalOffset < sizeY)
        {
            drawlinenocopy(i, 0);
            
            i++;
        }*/
        
        move(yCache, xCache);
        actualX = actualXCache;
        drawrows();
        
        processchar(sizeY, sizeX, 277967);

        refresh();
    }
    else if (charin == 23) // ctrl+w
    {
        //fprintf(stderr, "ctrl+w");
        //saveandclose();
        return 2;
    }
    else if (charin == 24) // ctrl+x
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
        insertchar(y - 1 + verticalOffset, x + rowOffset, newchar);
        //free(newchar);
        processchar(sizeY, sizeX, 277967);
        //move(y, x + 1);
        //actualX++;
        //xRequest = actualX;
        
        //drawlinenocopy(y - 1, calculateoffset(y - 1));
        /*if (y + verticalOffset < numberOfRows)
        {
            int actualXCache = actualX;
            actualX = 0;
            int xCache, yCache;
            getyx(stdscr, yCache, xCache);
            move(y, 0);
            drawlinenocopy(y + verticalOffset, 0);
            move(yCache, xCache);
            actualX = actualXCache;
        }*/
        //fprintf(stderr, "%s \n", rows[y - 1].chars);
        
        refresh();
    }
    else if (charin == 8 || charin == 127) // backspace
    {
        int y,x;
        getyx(stdscr, y, x);
        if (actualX == 0 && x == 0)
        {
            if (y + verticalOffset == 1)
            {
                return 0;
            }
            if (y + verticalOffset == numberOfRows)
            {
                //insertrow(y + verticalOffset, "", 0);
            }
            
            long nextActualX = rows[y - 1 + verticalOffset].displaylength; //strlen(rows[y - 1].chars);
            
            pstring newrow = pstr_combine(rows[y - 2 + verticalOffset].chars, rows[y - 1 + verticalOffset].chars);
            
            removerow(y - 1 + verticalOffset);
            rows[y - 2 + verticalOffset].chars = newrow;
            rows[y - 2 + verticalOffset].displaylength = calculatedisplaylength(y - 2 + verticalOffset);
            
            long i = 0;
            long actualXCache = actualX;
            actualX = 0;
            
            move(numberOfRows + 1 - verticalOffset, 0);
            clrtoeol();
            
            move(y, 0);
            
            actualX = 0;
            xRequest = actualX;
            i = 0;
            while (i <= nextActualX)
            {
                processchar(sizeY, sizeX, 277968);
                i++;
            }
            
            //removerow(y + verticalOffset);
            drawrows();
            //actualX = nextActualX;
        }
        else
        {
            int y,x;
            getyx(stdscr, y, x);
            deletechar(y - 1 + verticalOffset, x);
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

int calculatedisplaylength(long row)
{
    long i = 0;//rows[row].reallength - 1;
    long newlen = 0;
    while (i < rows[row].chars.len)
    {
        if (rows[row].chars.chars[i] == '\t')
        {
            newlen += calculatetabwidth(newlen);//, false);//i, true);
        }
        else
        {
            newlen++;
        }

        i++;
    }

    return newlen;
}

int drawrows()
{
    int sizeY, sizeX;
    getsize(&sizeX, &sizeY);
    
    long i = verticalOffset;
    
    while (i - verticalOffset + 3 < sizeY && i < numberOfRows)
    {
        drawlinenocopy(i, 0);
        i++;
    }
}

int calculateoffset(long row, bool movingright)
{
    int sizeY, sizeX;
    getsize(&sizeX, &sizeY);
    
    if (rows[row].displaylength <= sizeX)
    {
        return 0;
    }
    //else if (actualX < sizeX - 8)
    //{
    //    return 0;
    //}
    else
    {
        long i = 0;
        int y,x;
        //getsize(&x, &y);
        getyx(stdscr, y, x);

        /*if (actualX - x < 7)
        {
            return 8 * (x / 8);
        }*/
        
        if (movingright)
        {
            if (x < sizeX - 2)
            {
                i = rowOffset;
            }
            else
            {
                while (i < (actualX) - (sizeX - 2))
                {
                    i += 1;
                }
            }
        }
        else
        {
            if (x > 1)
            {
                i = rowOffset;
            }
            else
            {
                while (i < (actualX) - 1)
                {
                    i += 1;
                }
            }
        }
        
        if (rows[row].displaylength - i < sizeX)
        {
            i = rows[row].displaylength - sizeX + 8;
        }
        
        return i;
    }
}

int drawlinenocopy(long row, long offset)
{
    /*int y,x;
    getyx(stdscr, y, x);
    int sizeY, sizeX;
    getsize(&sizeX, &sizeY);*/
    //char *text = malloc(rows[row].length);//malloc(strlen(rows[row].chars));
    //fprintf(stderr, "row %d reallen: %d\n", row, rows[row].reallength);
    //char *text = calloc(1, rows[row].reallength * sizeof(char));
    //char text[rows[row].reallength];
    //strncpy(text, rows[row].chars, rows[row].reallength);
    pstring tmp = rows[row].chars;
    //text[rows[row].reallength - 1] = '\0';
    /*if (actualX < sizeX)
    {
        drawline(row, text, 0);
    }
    else
    {
        drawline(row, text, actualX - x);
    }*/
    //drawline(row, text, offset, rows[row].length);
    drawline(row, tmp, offset);
    //free(text);
}

int drawline(long row, pstring line, long offset)
{
    //free(rows[row].chars);
    //rows[row].chars = malloc(linelen);//strlen(line));
    //rows[row].chars = realloc(rows[row].chars, linelen * sizeof(char));
    rows[row].chars = line;
    if (row >= numberOfRows)
    {
        fprintf(stderr, "out of range in drawline");
        //return 1;
    }
    //strncpy(rows[row].chars, line, linelen);
    //rows[row].reallength = linelen;//strlen(line);
    rows[row].displaylength = calculatedisplaylength(row);

    pstring text = pstr_get_terminated(rows[row].chars);
    
    int sizeY, sizeX;
    getsize(&sizeX, &sizeY);
    
    if (row + 1 - verticalOffset - sizeY > -3)
    {
        return 1;
    }

    if (text.len - 1 < sizeX) // the -1 is because of the \0 at the end (from the pstr_get_terminated() above)
    {
        writespecificline(row + 1 - verticalOffset, text);
    }
    else
    {
        pstring tmp = pstr_start_at(text, offset);
        
        writespecificline(row + 1 - verticalOffset, tmp);
        
        if (offset > 0)
        {
            //tmp[0] = '<';
            writespecificcharswithcolor(row + 1 - verticalOffset, 0, "<", COLOR_BLACK, COLOR_WHITE);
        }
        if (offset + sizeX < rows[row].chars.len)//strlen(rows[row].chars))
        {
            //tmp[sizeX - 1] = '>';
            writespecificcharswithcolor(row + 1 - verticalOffset, sizeX - 1, ">", COLOR_BLACK, COLOR_WHITE);
        }
        
        //free(tmp);
    }
    // implement scrolling in the future
}

//https://stackoverflow.com/questions/12733105/c-function-that-counts-lines-in-file
/*int countlines(char *filename)
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
}*/

int insertrow(long insertbeforeindex, pstring row)//, char * rows2)
{
    expandrows();
    
    long i = numberOfRows - 1;
    //int oldlen = strlen(rows2);
    
    //rows[numberOfRows - 1].chars = malloc(strlen(row));
    
    while (i >= insertbeforeindex + 1)
    {
        //printf("%s %d", "i=", i);
        //free(rows[i].chars);
        //rows[i].chars = malloc(rows[i - 1].reallength);//malloc(strlen(rows[i - 1].chars));
        //rows[i].chars = realloc(rows[i].chars, rows[i - 1].reallength * sizeof(char));
        //rows[i].reallength = rows[i - 1].reallength;
        //rows[i].displaylength = rows[i - 1].displaylength;
        //strncpy(rows[i].chars, rows[i - 1].chars, rows[i - 1].reallength);//strlen(rows[i - 1].chars));
        
        rows[i].chars = rows[i - 1].chars;
        rows[i].displaylength = rows[i - 1].displaylength;

        i--;
    }
    //memmove(rows[insertbeforeindex + 1].chars, rows[insertbeforeindex].chars, (numberOfRows - insertbeforeindex - 1) * sizeof(char*));
    //memmove(rows[insertbeforeindex + 1].reallength, rows[insertbeforeindex].reallength, (numberOfRows - insertbeforeindex - 1) * sizeof(long));

    //free(rows[insertbeforeindex].chars);
    //free(rows[insertbeforeindex].chars);
    //rows[insertbeforeindex].chars = malloc(rowlen);//strlen(row));
    //rows[insertbeforeindex].reallength = rowlen;//strlen(row);
    //strncpy(rows[insertbeforeindex].chars, row, rowlen);//strlen(row));
    rows[insertbeforeindex].chars = row;
    rows[insertbeforeindex].displaylength = calculatedisplaylength(insertbeforeindex);
}
int expandrows()
{
    //struct editorRow * tmp = &rows;
    //tmp = realloc(tmp, (numberOfRows + 1) * sizeof(editorRow));
    
    /*editorRow *tmp;
    
    tmp = malloc(numberOfRows * sizeof(editorRow));
    
    long i = 0;
    
    while (i < numberOfRows)
    {
        //printf("%s", rows[i].chars);
        //tmp[i].chars = malloc(sizeof(editorRow));
        tmp[i].chars = malloc(rows[i].reallength);//malloc(strlen(rows[i].chars));
        tmp[i].reallength = rows[i].reallength;
        tmp[i].displaylength = rows[i].displaylength;
        strncpy(tmp[i].chars, rows[i].chars, rows[i].reallength);//strlen(rows[i].chars));
        //printf("%s", tmp[i].chars);
        i++;
    }*/
    
    rows = realloc(rows, (numberOfRows + 1) * sizeof(editorRow));
    //rows = malloc((numberOfRows + 1) * sizeof(editorRow));
    
    /*i = 0;
    
    while (i < numberOfRows)
    {
        //printf("%s", tmp[i].chars);
        rows[i].chars = malloc(tmp[i].reallength);//malloc(strlen(tmp[i].chars));
        rows[i].reallength = tmp[i].reallength;
        rows[i].displaylength = tmp[i].displaylength;
        strncpy(rows[i].chars, tmp[i].chars, tmp[i].reallength);//strlen(tmp[i].chars));
        //printf("%s", rows[i].chars);
        i++;
    }*/
    
    //rows = tmp;
    
    numberOfRows++;
}

int removerow(long index)
{
    //struct editorRow * tmp = &rows;
    //tmp = realloc(tmp, (numberOfRows + 1) * sizeof(editorRow));
    
    editorRow tmp[numberOfRows];
    
    long i = 0;
    
    while (i < index)
    {
        tmp[i].chars = rows[i].chars;
        tmp[i].displaylength = rows[i].displaylength;
        i++;
    }
    i++;
    while (i < numberOfRows)
    {
        tmp[i - 1].chars = rows[i].chars;
        tmp[i - 1].displaylength = rows[i].displaylength;
        i++;
    }
    
    //free(rows);
    //rows = malloc((numberOfRows - 1) * sizeof(editorRow));
    rows = realloc(rows, (numberOfRows - 1) * sizeof(editorRow));

    i = 0;
    
    while (i < numberOfRows - 1)
    {
        rows[i].chars = tmp[i].chars;
        rows[i].displaylength = tmp[i].displaylength;
        i++;
    }
    
    //rows = tmp;
    
    numberOfRows--;
    
    //free(tmp);
}

int calculatedisplaypos(long row, long arrayindex)
{
    long i = 0;//rows[row].reallength - 1;
    long newlen = 0;
    while (i < arrayindex)
    {
        if (arrayindex == 0)
        {
            i = 0;
            break;
        }
        if (rows[row].chars.chars[i] == '\t')
        {
            newlen += 8 - newlen % 8;//(i + 8) / 8 * 8 - i;
        }
        else
        {
            newlen++;
        }

        i++;
        /*if (i >= arrayindex)
        {
            if (i > arrayindex)
            {
                //fprintf(stderr, "1210 error");
            }
            break;
        }*/
    }

    return(newlen);
}

int calculatearraypos(long row, long index)
{
    long i = 0;//rows[row].reallength - 1;
    long newlen = 0;
    while (i < rows[row].chars.len)
    {
        if (index == 0)
        {
            i = 0;
            break;
        }
        if (rows[row].chars.chars[i] == '\t')
        {
            newlen += 8 - newlen % 8;//(i + 8) / 8 * 8 - i;
        }
        else
        {
            newlen++;
        }

        i++;
        if (newlen >= index)
        {
            if (newlen > index)
            {
                //fprintf(stderr, "1210 error");
            }
            break;
        }
    }

    return(i);
}

int calculatetabwidth(long index)//, bool realpos)
{
    return 8 - index % 8;
}

int insertchar(long row, long insertbeforeindex, char insertchar)
{
    rows[row].chars = pstr_insert_char(rows[row].chars, insertchar, calculatearraypos(row, insertbeforeindex));
    rows[row].displaylength = calculatedisplaylength(row);
}

int deletechar(long row, long deleteindex)
{
    long i = calculatearraypos(row, deleteindex);

    i--;

    rows[row].chars = pstr_remove_char(rows[row].chars, deleteindex);
    rows[row].displaylength = calculatedisplaylength(row);
}

int getsize(int *x, int *y) {
    getmaxyx(stdscr, *y, *x);
    return 0;
}

void writespecificline(long line, pstring chars) {
    int y,x;
    getyx(stdscr, y, x);
    move(line, 0);
    init_pair(2, COLOR_WHITE, COLOR_BLACK);
    attrset(COLOR_PAIR(2));
    clrtoeol();
    addstr(pstr_get_terminated(chars).chars);
    attroff(COLOR_PAIR(2));
    move(y, x);
}
void writespecificlinewithcolor(long line, pstring chars, int textcolor, int backgroundcolor) {
    int y,x;
    getyx(stdscr, y, x);
    init_pair(1, textcolor, backgroundcolor);
    attrset(COLOR_PAIR(1));
    move(line, 0);
    clrtoeol();
    addstr(pstr_get_terminated(chars).chars);
    attroff(COLOR_PAIR(1));
    move(y, x);
}

void writespecificcharswithcolor(long line, long charindex, char *chars, int textcolor, int backgroundcolor) {
    int y,x;
    getyx(stdscr, y, x);
    init_pair(1, textcolor, backgroundcolor);
    attrset(COLOR_PAIR(1));
    move(line, charindex);
    addstr(chars);
    attroff(COLOR_PAIR(1));
    move(y, x);
}
