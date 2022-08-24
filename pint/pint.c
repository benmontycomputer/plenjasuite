#include "pint.h"

#define DEFAULT_FILENAME ".pinttmp"
#define TABLEN 8

#define TBARH 1
#define BBARH 2

#define BBARM "^X: Quit    ^W: Write"
#define BBARI "^X          ^W       "

#define SAVEMSG "Do you want to save? (y)es/(n)o/(c)ancel\0"
#define SFAIL_EACCES "Failed to save: permission denied.\0"
#define SFAIL_GENERIC "Failed to save.\0"
#define SFAIL_INVALIDINPUT "Invalid input. Not closing.\0"

typedef struct fileLines
{
    pstring *lines;
    long lineCount;
} fileLines;

typedef struct display
{
    long firstRow;
    long rowCap;
    long currentRow;
    long widthCap;
    long xIndex;
    long xOffset;
} display;

static struct fileLines lines;
static struct display currentDisplay;

int main(int argc, char **argv) {
    char *filename;

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
	
    setlocale(LC_ALL, "en_US.UTF-8");
	initscr();

    int returnVal = edit(filename);

    endwin();
    return returnVal;
}

int edit(char *filename)
{
    if (start_color() != OK)
	{
		printf("Couldn't start color.");
		return 2;
	}

    clear();
	noecho();
	cbreak();
    keypad(stdscr, true);

    move(TBARH, 0);
    
    currentDisplay.currentRow = 0;
    currentDisplay.firstRow = 0;
    currentDisplay.xIndex = 0;

    getmaxyx(stdscr,currentDisplay.rowCap,currentDisplay.widthCap);
    currentDisplay.rowCap -= (TBARH + BBARH);

    lines.lines = openFile(filename);
    if (countLines(filename) == 0 | lines.lines == NULL)
    {
        lines.lines = malloc(sizeof(pstring));
        lines.lineCount = 1;
        lines.lines[0] = pstr_new();
    }
    else
    {
        lines.lineCount = countLines(filename);
    }

    draw();

    int maxY = getmaxy(stdscr);

    writeLineInvertColor(pstr_new_from_chars(" Pint Editor ", 13), 0);
    writeLine(pstr_new_from_chars(BBARM, strlen(BBARM)), maxY - BBARH);

    int i = 0;
    while (i < strlen(BBARI))
    {
        if (BBARI[i] != ' ')
        {
            char *tmp = malloc(2);
            tmp[0] = BBARI[i];
            tmp[1] = '\0';
            writeCharsInvertColor(pstr_new_from_chars(tmp, 2), maxY - BBARH, i);
            free(tmp);
        }
        i ++;
    }

    while (true)
    {
        int ch = getch();
        long oldY, oldX;
        getyx(stdscr, oldY, oldX);

        if (ch == KEY_LEFT)
        {
            if (currentDisplay.xIndex == 0)
            {
                if (currentDisplay.currentRow == 0)
                {
                    // at beginning
                }
                else
                {
                    if (currentDisplay.currentRow == currentDisplay.firstRow)
                    {
                        currentDisplay.firstRow --;
                    }
                    currentDisplay.currentRow --;
                    currentDisplay.xIndex = lines.lines[currentDisplay.currentRow].len;
                    currentDisplay.xOffset = -1;
                    draw();
                }
            }
            else
            {
                if (currentDisplay.xIndex - currentDisplay.xOffset <= 1)
                {
                    currentDisplay.xOffset --;
                    if (currentDisplay.xOffset <= 0)
                    {
                        currentDisplay.xOffset == 0;
                    }
                }
                currentDisplay.xIndex--;
                draw();
            }
        }
        else if (ch == KEY_RIGHT)
        {
            if (currentDisplay.xIndex >= lines.lines[currentDisplay.currentRow].len)
            {
                if (currentDisplay.currentRow + 1 >= lines.lineCount)
                {
                    // EOF
                }
                else
                {
                    if (oldY + 1 >= getmaxy(stdscr) - BBARH)
                    {
                        currentDisplay.firstRow ++;
                    }
                    currentDisplay.currentRow++;
                    currentDisplay.xIndex = 0;
                    currentDisplay.xOffset = 0;
                    draw();
                }
            }
            else
            {
                if (displayPos(lines.lines[currentDisplay.currentRow], currentDisplay.xIndex) - displayPos(lines.lines[currentDisplay.currentRow], currentDisplay.xOffset) + 1 >= getmaxx(stdscr) - 1)
                {
                    currentDisplay.xOffset ++;
                }
                currentDisplay.xIndex++;
                draw();
            }
        }
        else if (ch == KEY_DOWN)
        {
            if (currentDisplay.currentRow + 1 >= lines.lineCount)
            {
                
            }
            else
            {
                currentDisplay.currentRow++;

                if (TBARH + currentDisplay.rowCap <= oldY + 1)
                {
                    currentDisplay.firstRow++;
                }
                else
                {
                    
                }
                currentDisplay.xOffset = 0;
                currentDisplay.xIndex = 0;
                draw();
            }
        }
        else if (ch == KEY_UP)
        {
            if (currentDisplay.currentRow <= 0)
            {

            }
            else
            {
                currentDisplay.currentRow--;
                if (oldY <= TBARH)
                {
                    currentDisplay.firstRow--;
                }
                else
                {

                }
                currentDisplay.xOffset = 0;
                currentDisplay.xIndex = 0;
                draw();
            }
        }
        else if (isalnum(ch) || ch == ' ' || ch == '!' || ch == '@' || ch == '#' || ch == '$' || ch == '%' || ch == '^' || ch == '&' || ch == '*' || ch == '(' || ch == ')' || ch == '-' || ch == '_' || ch == '=' || ch == '+' || ch == '`' || ch == '~' || ch == '[' || ch == ']' || ch == '{' || ch == '}' || ch == '\\' || ch == '|' || ch == '\'' || ch == '"' || ch == ';' || ch == ':' || ch == ',' || ch == '.' || ch == '<' || ch == '>' || ch == '/' || ch == '?')
        {
            insertChar(currentDisplay.currentRow, ch, currentDisplay.xIndex);
            currentDisplay.xIndex++;
            draw();
        }
        else if (ch == 127)
        {
            if (currentDisplay.xIndex == 0)
            {
                if (currentDisplay.currentRow != 0)
                {
                    currentDisplay.xOffset = -1;
                    currentDisplay.xIndex = lines.lines[currentDisplay.currentRow - 1].len;
                    combineRows(currentDisplay.currentRow - 1, currentDisplay.currentRow);
                    currentDisplay.currentRow --;
                }
            }
            else
            {
                removeChar(currentDisplay.currentRow, currentDisplay.xIndex - 1);
                currentDisplay.xIndex --;
            }
            draw();
        }
        else if (ch == 10)
        {
            splitRow(currentDisplay.currentRow, currentDisplay.xIndex);
            currentDisplay.xIndex = 0;
            currentDisplay.xOffset = 0;
            currentDisplay.currentRow ++;
            draw();
        }
        else if (ch == 24)
        {
            //save(filename);
            //stdMessage(pstr_new_from_chars(" Saved ", 7));
            int sizeY, sizeX;
            getmaxyx(stdscr, sizeY, sizeX);
            stdMessage(pstr_new_from_chars(SAVEMSG, strlen(SAVEMSG)));
            refresh();
            int ch = getch();
            if (ch == 'y')
            {
                int result = save(filename);
                if (result == 0)
                {
                    return 0;
                }
                else
                {
                    if (result == EACCES)
                    {
                        stdMessage(pstr_new_from_chars(SFAIL_EACCES, strlen(SFAIL_EACCES)));
                    }
                    else
                    {
                        stdMessage(pstr_new_from_chars(SFAIL_GENERIC, strlen(SFAIL_GENERIC)));
                    }
                    refresh();
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

                move(sizeY - BBARH - 1, 0);
                clrtoeol();
                move(y, x);
                draw();
            }
            else
            {
                int y,x;
                getyx(stdscr, y, x);

                move(sizeY - BBARH - 1, 0);
                clrtoeol();
                move(y, x);
                refresh();
                
                stdMessage(pstr_new_from_chars(SFAIL_INVALIDINPUT, strlen(SFAIL_INVALIDINPUT)));
            }
        }
        else
        {
            /*char *t1 = malloc(10);
            snprintf(t1, 10, "%d", ch);
            writeLineInvertColor(pstr_new_from_chars(t1, 10), 0);
            free(t1);*/
        }
    }
}

int stdMessage(pstring msg)
{
    int sizeY, sizeX;
    getmaxyx(stdscr, sizeY, sizeX);
    int buffersize = (sizeX - msg.len) / 2;

    int x,y;
    getyx(stdscr, y, x);
    move(sizeY - 3, 0);
    clrtoeol();
    move(y, x);

    if (msg.len + 1 < sizeX)
    {
        pstring output = pstr_new_from_chars("[", 2);
        output = pstr_combine(output, msg);
        output = pstr_combine(output, pstr_new_from_chars("]", 2));
        writeCharsInvertColor(output, sizeY - BBARH - 1, buffersize - 1);
    }
    else
    {
        writeCharsInvertColor(msg, sizeY - BBARH - 1, buffersize);
    }

    refresh();
}

int save(char *filename)
{
    writeFile(filename, lines.lines, lines.lineCount);
}

int combineRows(long keep, long remove)
{
    lines.lines[keep] = pstr_combine(lines.lines[keep], lines.lines[remove]);
    
    removeRow(remove);
}

int splitRow(long row, long index)
{
    insertRow(pstr_start_at(lines.lines[row], index), row + 1);
    lines.lines[row] = pstr_first_n(lines.lines[row], index);
}

int insertRow(pstring contents, long index)
{
    expandRows();

    long i = lines.lineCount - 1;
    
    while (i >= index + 1)
    {
        lines.lines[i] = lines.lines[i - 1];

        i--;
    }
    
    lines.lines[index] = contents;
}

int expandRows()
{
    lines.lines = realloc(lines.lines, (lines.lineCount + 1) * sizeof(pstring));
    lines.lineCount ++;
}

int removeRow(long index)
{
    pstring tmp[lines.lineCount];
    
    long i = 0;
    
    while (i < index)
    {
        tmp[i].chars = lines.lines[i].chars;
        //tmp[i].displaylength = rows[i].displaylength;
        i++;
    }
    i++;
    while (i < lines.lineCount)
    {
        tmp[i - 1].chars = lines.lines[i].chars;
        //tmp[i - 1].displaylength = rows[i].displaylength;
        i++;
    }
    
    //free(rows);
    //rows = malloc((numberOfRows - 1) * sizeof(editorRow));
    lines.lines = realloc(lines.lines, (lines.lineCount - 1) * sizeof(pstring));

    i = 0;
    
    while (i < lines.lineCount - 1)
    {
        lines.lines[i].chars = tmp[i].chars;
        //rows[i].displaylength = tmp[i].displaylength;
        i++;
    }
    
    //rows = tmp;
    
    lines.lineCount --;
}

int insertChar(long row, char insertChar, long pos)
{
    lines.lines[row] = pstr_insert_char(lines.lines[row], insertChar, pos);
}

int removeChar(long row, long pos)
{
    lines.lines[row] = pstr_remove_char(lines.lines[row], pos);
}

int draw()
{
    long newY = currentDisplay.currentRow - currentDisplay.firstRow + TBARH;
    long newX = 0;
    long xIndexPos = displayPos(lines.lines[currentDisplay.currentRow], currentDisplay.xIndex);
    if (currentDisplay.xOffset == -1)
    {
        if (xIndexPos >= getmaxx(stdscr) - 1)
        {
            currentDisplay.xOffset = currentDisplay.xIndex - getmaxx(stdscr) + 8;
        }
        else
        {
            currentDisplay.xOffset = 0;
        }
    }

    while (xIndexPos - displayPos(lines.lines[currentDisplay.currentRow], currentDisplay.xOffset) >= getmaxx(stdscr) - 1)
    {
        currentDisplay.xOffset++;
    }

    newX = xIndexPos - displayPos(lines.lines[currentDisplay.currentRow], currentDisplay.xOffset);

    move(newY, newX);

    long firstLine = currentDisplay.firstRow;
    long lastLine = currentDisplay.firstRow + currentDisplay.rowCap;

    long i = firstLine;
    while (i < lastLine)
    {
        if (i >= lines.lineCount)
        {
            writeLine(pstr_new_from_chars("", 0), i - firstLine + TBARH);
        }
        else
        {
            if (i == currentDisplay.currentRow)
            {
                writeLine(pstr_start_at(lines.lines[i], currentDisplay.xOffset), i - firstLine + TBARH);
            }
            else
            {
                writeLine(lines.lines[i], i - firstLine + TBARH);
            }
        }

        i++;
    }
    refresh();
}

int writeLine(pstring line, int y)
{
    int oldY = getcury(stdscr);
    int oldX = getcurx(stdscr);
    move(y, 0);
    clrtoeol();

    addnstr(pstr_get_terminated(line).chars, getmaxx(stdscr));

    move(oldY, oldX);
}

int writeLineInvertColor(pstring line, int y)
{
    bool invertColor = true;
    if (invertColor)
    {
        init_pair(1, COLOR_BLACK, COLOR_WHITE);
        attrset(COLOR_PAIR(1));
    }
    int oldY = getcury(stdscr);
    int oldX = getcurx(stdscr);
    move(y, 0);
    clrtoeol();

    addnstr(pstr_get_terminated(line).chars, getmaxx(stdscr));

    if (invertColor)
    {
        attroff(COLOR_PAIR(1));
    }

    move(oldY, oldX);
}

int writeCharsInvertColor(pstring chars, int line, int charindex)
{
    int y,x;
    getyx(stdscr, y, x);
    init_pair(1, COLOR_BLACK, COLOR_WHITE);
    attrset(COLOR_PAIR(1));
    move(line, charindex);
    addnstr(pstr_get_terminated(chars).chars, getmaxx(stdscr));
    attroff(COLOR_PAIR(1));
    move(y, x);
}

long displayLen(pstring row)
{
    long i = 0;
    long len = 0;
    while (i < row.len)
    {
        if (row.chars[len] == '\t')
        {
            len += TABLEN;
        }
        else
        {
            len ++;
        }
        i++;
    }
    return len;
}
long displayPos(pstring row, long arrayIndex)
{
    long i = 0;
    long pos = 0;
    while (i < arrayIndex)
    {
        if (i >= row.len)
        {
            return pos;
        }
        if (row.chars[pos] == '\t')
        {
            pos += TABLEN;
        }
        else
        {
            pos ++;
        }
        i++;
    }
    return pos;
}