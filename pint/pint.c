#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<ncurses.h>
#include<ctype.h>
#include<signal.h>
#include<errno.h>
#include "pint.h"

#define TMPREFIX " Pint Editor - "
#define TABSIZE 8

/*int getsize(int *x, int *y);
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

int calculateoffset(int row, bool movingright);*/

typedef struct editorRow
{
    //char *chars;
    char *chars;
    int displaylength;
    int reallength;
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
            printf("pint v0.0.1-alpha.2\n");
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
    //sigaction(SIGTERM, &sa, NULL);
    
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
    writespecificline(sizeY - 2, bottom1);
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
                save();
                return;
            }
            else if (ch == 'n')
            {
                return;
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
    char filename1[strlen(filename)];
    strcpy(filename1, filename);
    
    FILE *file = fopen(filename1, "w");
    if (file)
    {
        int i = 0;
        while (i < numberOfRows)
        {
            //char tmp[rows[i].length];
            char *tmp = malloc(rows[i].reallength + 2);
            strncpy(tmp, rows[i].chars, rows[i].reallength);
            tmp[rows[i].reallength] = '\n';
            tmp[rows[i].reallength + 1] = '\0';
            //fputs(rows[i].chars, file);
            fputs(tmp, file);
            free(tmp);
            //fputs('\n', file);
            
            //printf("adding line");
            
            i++;
        }
        
        //file.close();
        fclose(file);
        
        //endwin();
    }
    else
    {
        int sizeY, sizeX;
        getsize(&sizeX, &sizeY);
        //fprintf(stderr, "%s %s", "failed to save: ", filename1);
        stdmessage(sizeY, sizeX, "Failed to save.");
        refresh();
    }
    
    //free(file);
}

int stdmessage(int sizeY, int sizeX, char *message)
{
    int buffersize = (sizeX - strlen(message)) / 2;

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
                int lentmp = rows[y + verticalOffset - 2].displaylength;//strlen(rows[y - 2].chars);
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
            int i = 1;
            int actualXOld = actualX;
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
            if (rows[y - 1 + verticalOffset].chars[calculatearraypos(y - 1 + verticalOffset, actualX) - 1] == '\t')
            {
                actualX -= calculatetabwidth(y - 1 + verticalOffset, calculatedisplaypos(y - 1, calculatearraypos(y - 1 + verticalOffset, actualX) - 1));//, false);
            }
            else
            {
                actualX--;
            }
            xRequest = actualX;
            //fprintf(stderr, "%d ", rowOffset);
            //move(y, actualX - rowOffset);
            int rowOffsetOld = rowOffset;
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
            int i = calculatearraypos(y - 1 + verticalOffset, actualX);
            if (rows[y - 1 + verticalOffset].chars[i] == '\t')
            {
                actualX += calculatetabwidth(y - 1 + verticalOffset, actualX);//, false);
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
        int tmpint = rows[y - 1 + verticalOffset].displaylength - actualX + 1;
        
        //if (y + verticalOffset >= numberOfRows)
        //{
        //    return 0;
        //}
        
        int xRequestCache = xRequest;
        if (xRequest > rows[y + verticalOffset].displaylength)
        {
            tmpint += rows[y + verticalOffset].displaylength;
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
            //tmpint += 
            return;
        }
        else
        {
            if (xRequest < rows[y - 2 + verticalOffset].displaylength)
            {
                //tmpint += rows[y - 2].length;
                tmpint += rows[y - 2 + verticalOffset].displaylength - xRequest;
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
            if (y == 1)
            {
                return;
            }
            if (y == numberOfRows)
            {
                insertrow(y, "");
            }
            
            int nextActualX = rows[y - 1 + verticalOffset].displaylength; //strlen(rows[y - 1].chars);
            
            char *tmp;
            
            tmp = malloc(rows[y - 2 + verticalOffset].reallength + rows[y - 1 + verticalOffset].reallength + 1); //malloc(strlen(rows[y - 2].chars) + strlen(rows[y - 1].chars) + 1);
            
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

int calculatedisplaylength(int row)
{
    int i = 0;//rows[row].reallength - 1;
    int newlen = 0;
    while (i < rows[row].reallength)
    {
        if (rows[row].chars[i] == '\t')
        {
            newlen += calculatetabwidth(row, newlen);//, false);//i, true);
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
        int i = 0;
        int y,x;
        //getsize(&x, &y);
        getyx(stdscr, y, x);

        /*if (actualX - x < 7)
        {
            return 8 * (x / 8);
        }*/
        
        if (movingright)
        {
            if (x < sizeX - 1)
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

int drawlinenocopy(int row, int offset)
{
    /*int y,x;
    getyx(stdscr, y, x);
    int sizeY, sizeX;
    getsize(&sizeX, &sizeY);*/
    //char *text = malloc(rows[row].length);//malloc(strlen(rows[row].chars));
    char text[rows[row].reallength];
    strcpy(text, rows[row].chars);
    /*if (actualX < sizeX)
    {
        drawline(row, text, 0);
    }
    else
    {
        drawline(row, text, actualX - x);
    }*/
    //drawline(row, text, offset, rows[row].length);
    drawline(row, text, offset, rows[row].reallength);
    //free(text);
}

int drawline(int row, char *line, int offset, int linelen)
{
    rows[row].chars = malloc(linelen);//strlen(line));
    strncpy(rows[row].chars, line, linelen);
    rows[row].reallength = linelen;//strlen(line);
    rows[row].displaylength = calculatedisplaylength(row);

    char text[linelen + 1];
    strncpy(text, rows[row].chars, rows[row].reallength);
    text[linelen] = '\0';
    
    int sizeY, sizeX;
    getsize(&sizeX, &sizeY);
    
    if (row + 1 - verticalOffset - sizeY > -3)
    {
        return;
    }

    if (linelen < sizeX)//strlen(line) < sizeX)
    {
        writespecificline(row + 1 - verticalOffset, text);
    }
    else
    {
        char *tmp = malloc(sizeX);
        
        strncpy(tmp, text+offset, sizeX);
        
        writespecificline(row + 1 - verticalOffset, tmp);
        
        if (offset > 0)
        {
            //tmp[0] = '<';
            writespecificcharswithcolor(row + 1 - verticalOffset, 0, "<", COLOR_BLACK, COLOR_WHITE);
        }
        if (offset + sizeX < rows[row].reallength)//strlen(rows[row].chars))
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
        rows[i].chars = malloc(rows[i - 1].reallength);//malloc(strlen(rows[i - 1].chars));
        rows[i].reallength = rows[i - 1].reallength;
        rows[i].displaylength = rows[i - 1].displaylength;
        memcpy(rows[i].chars, rows[i - 1].chars, rows[i - 1].reallength);//strlen(rows[i - 1].chars));
        
        i--;
    }
    
    rows[insertbeforeindex].chars = malloc(strlen(row));
    rows[insertbeforeindex].reallength = strlen(row);
    memcpy(rows[insertbeforeindex].chars, row, strlen(row));
    rows[insertbeforeindex].displaylength = calculatedisplaylength(insertbeforeindex);
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
        tmp[i].chars = malloc(rows[i].reallength);//malloc(strlen(rows[i].chars));
        tmp[i].reallength = rows[i].reallength;
        tmp[i].displaylength = rows[i].displaylength;
        strncpy(tmp[i].chars, rows[i].chars, rows[i].reallength);//strlen(rows[i].chars));
        //printf("%s", tmp[i].chars);
        i++;
    }
    
    //rows = realloc(rows, (numberOfRows + 1) * sizeof(editorRow));
    rows = malloc((numberOfRows + 1) * sizeof(editorRow));
    
    i = 0;
    
    while (i < numberOfRows)
    {
        //printf("%s", tmp[i].chars);
        rows[i].chars = malloc(tmp[i].reallength);//malloc(strlen(tmp[i].chars));
        rows[i].reallength = tmp[i].reallength;
        rows[i].displaylength = tmp[i].displaylength;
        strncpy(rows[i].chars, tmp[i].chars, tmp[i].reallength);//strlen(tmp[i].chars));
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
        tmp[i].chars = malloc(rows[i].reallength);//malloc(strlen(rows[i].chars));
        tmp[i].reallength = rows[i].reallength;
        tmp[i].displaylength = rows[i].displaylength;
        strncpy(tmp[i].chars, rows[i].chars, rows[i].reallength);//strlen(rows[i].chars));
        //printf("%s", tmp[i].chars);
        i++;
    }
    i++;
    while (i < numberOfRows)
    {
        //printf("%s", rows[i].chars);
        tmp[i - 1].chars = malloc(rows[i].reallength);//malloc(strlen(rows[i].chars));
        tmp[i - 1].reallength = rows[i].reallength;
        tmp[i - 1].displaylength = rows[i].displaylength;
        strncpy(tmp[i - 1].chars, rows[i].chars, rows[i].reallength);//strlen(rows[i].chars));
        //printf("%s", tmp[i].chars);
        i++;
    }
    
    rows = malloc((numberOfRows - 1) * sizeof(editorRow));
    
    i = 0;
    
    while (i < numberOfRows - 1)
    {
        //printf("%s", tmp[i].chars);
        rows[i].chars = malloc(tmp[i].reallength);//malloc(strlen(tmp[i].chars));
        rows[i].reallength = tmp[i].reallength;
        rows[i].displaylength = tmp[i].displaylength;
        strncpy(rows[i].chars, tmp[i].chars, tmp[i].reallength);//strlen(tmp[i].chars));
        //printf("%s", rows[i].chars);
        i++;
    }
    
    //rows = tmp;
    
    numberOfRows--;
    
    //free(tmp);
}

int calculatedisplaypos(int row, int arrayindex)
{
    int i = 0;//rows[row].reallength - 1;
    int newlen = 0;
    while (i < arrayindex)
    {
        if (arrayindex == 0)
        {
            i = 0;
            break;
        }
        if (rows[row].chars[i] == '\t')
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

int calculatearraypos(int row, int index)
{
    int i = 0;//rows[row].reallength - 1;
    int newlen = 0;
    while (i < rows[row].reallength)
    {
        if (index == 0)
        {
            i = 0;
            break;
        }
        if (rows[row].chars[i] == '\t')
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

int calculatetabwidth(int row, int index)//, bool realpos)
{
    int i = 0;//rows[row].reallength - 1;
    int newlen = 0;
    //if (realpos)
    //{
        /*while (i < rows[row].reallength)
        {
            if (index == 0)
            {
                i = 0;
                break;
            }
            if (rows[row].chars[i] == '\t')
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
                    fprintf(stderr, "1172 error");
                }
                break;
            }
        }*/
        //i = calculatearraypos(row, index);
    //}
    /*else
    {
        i = index;
    }*/
    
    i = index;

    //int nexttabstop = (i + 8) / 8 * 8;
    /*if ((i / 8) * 8 != i && nexttabstop == 8)
    {
        nexttabstop = 0;
    }*/
    //return nexttabstop - i;
    /*fprintf(stderr, "\norig index value: %d\n", index);
    fprintf(stderr, "i value: %d\n", i);
    fprintf(stderr, "row display size: %d\n", rows[row].displaylength);
    fprintf(stderr, "tab return: %d\n\n", 8 - i % 8);*/
    return 8 - i % 8;
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
    //int realinsertbeforeindex;
    int i = 0;//rows[row].reallength - 1;
    int newlen = 0;
    /*while (i < rows[row].reallength)
    {
        if (insertbeforeindex == 0)
        {
            i = 0;
            break;
        }
        if (rows[row].chars[i] == '\t')
        {
            newlen += calculatetabwidth(row, i, true);
        }
        else
        {
            newlen++;
        }

        i++;
        if (newlen >= insertbeforeindex)
        {
            if (newlen > insertbeforeindex)
            {
                fprintf(stderr, "1276 error");
            }
            break;
        }
    }*/
    i = calculatearraypos(row, insertbeforeindex);

    //char *tmp = insert_char_malloc(rows[row].chars, rows[row].reallength/*strlen(rows[row].chars)*/, insertchar, insertbeforeindex);
    char *tmp = insert_char_malloc(rows[row].chars, rows[row].reallength/*strlen(rows[row].chars)*/, insertchar, i);
    //rows[row].length = rows[row].length + 1;//strlen(tmp);
    rows[row].chars = malloc(rows[row].reallength);//malloc(strlen(tmp));
    //memcpy(rows[row].chars, tmp, rows[row].length);
    strncpy(rows[row].chars, tmp, rows[row].reallength);
    rows[row].displaylength = calculatedisplaylength(row);
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
    //deleteindex--;

    int i = 0;//rows[row].reallength - 1;
    int newlen = 0;
    /*while (i < rows[row].reallength)
    {
        if (deleteindex == 0)
        {
            i = 0;
            break;
        }
        if (rows[row].chars[i] == '\t')
        {
            newlen += calculatetabwidth(row, i, true);
        }
        else
        {
            newlen++;
        }

        i++;
        if (newlen >= deleteindex)
        {
            break;
        }
    }*/
    i = calculatearraypos(row, deleteindex);

    i--;

    memmove(&rows[row].chars[i], &rows[row].chars[i + 1], rows[row].reallength - i);
    //rows[row].chars = realloc(rows[row].chars, strlen(rows[row].chars) - 1);
    newlen = rows[row].reallength - 1;
    char *tmp = malloc(newlen);//malloc(strlen(rows[row].chars) - 1);
    memcpy(tmp, rows[row].chars, newlen);//strlen(rows[row].chars) - 1);
    //rows[row].chars = malloc(strlen(tmp));
    //memcpy(rows[row].chars, tmp, strlen(tmp));
    free(rows[row].chars);
    rows[row].chars = malloc(newlen);
    //rows[row].chars = tmp;
    memcpy(rows[row].chars, tmp, newlen);
    rows[row].reallength = newlen;
    rows[row].displaylength = calculatedisplaylength(row);
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
    
    int newlen = rows[row].reallength + 1;
    newrow = malloc(newlen);//malloc((strlen(rows[row].chars)) + 1);
    
    strncpy(newrow, rows[row].chars, newlen - 1);//strlen(rows[row].chars));
    
    free(rows[row].chars);
    //rows[row].chars = realloc(rows[row].chars, strlen(newrow));
    rows[row].chars = newrow;
    rows[row].reallength = newlen;
    rows[row].displaylength = calculatedisplaylength(row);
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

void writespecificline(int line, char *chars) {
    //char *tmp = malloc(strlen(chars) + 1);
    //tmp[strlen(chars)] = '\0';
    int y,x;
    getyx(stdscr, y, x);
    move(line, 0);
    clrtoeol();
    addstr(chars);
    move(y, x);
}
void writespecificlinewithcolor(int line, char *chars, int textcolor, int backgroundcolor) {
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

void writespecificcharswithcolor(int line, int charindex, char *chars, int textcolor, int backgroundcolor) {
    int y,x;
    getyx(stdscr, y, x);
    init_pair(1, textcolor, backgroundcolor);
    attrset(COLOR_PAIR(1));
    move(line, charindex);
    addstr(chars);
    attroff(COLOR_PAIR(1));
    move(y, x);
}
