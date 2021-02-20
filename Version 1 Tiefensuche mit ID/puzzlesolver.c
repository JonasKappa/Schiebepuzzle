//Autor: J. Kappa
//calculates the stack mem usage
//only launch on PC with gcc -o ps puzzlesolverMem.c -D PC
//but should work on AKSEN as well, but wont be showing the mem usage

//Standard-Include-Files
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#ifndef PC
#include <stub.h>
#endif
#ifdef PC
#include <stdio.h>
#include <time.h>
#endif

#define MAX_DEPTH 30
#define AGENDA_LENGTH 33
#define PUZZLE_LENGTH 9
#define DEPTH_ITERATOR 1

#define CHAR 1
#define INT 2
#define LONG 2

#ifdef MEM
int memoryStack = 0;
int maxMem = 0;

void addMem(int n, int x)
{
    // printf("add ");
    memoryStack = memoryStack + (n * x);
    if (maxMem < memoryStack)
    {
        maxMem = memoryStack;
    }
}

void remMem(int n, int x)
{
    // printf("rem ");
    memoryStack = memoryStack - (n * x);
    if (memoryStack < 0)
    {
        printf("memoryStack is less then 0!!!!");
    }
}
#endif

// variables for checking if a solution is possible
int inversionNumberInitial = 0;
int inversionNumberFinal = 0;
bool solutionIsPossible = true;

// variables for agenda
char agenda[AGENDA_LENGTH][MAX_DEPTH];
int agendaPointer = 0;
bool agendaPointerOverflow = false;

// ### DIFFERENT PUZZLE COMBINATIONS ###

// char initialPuzzle[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'o'};
// char finalPuzzle[] = {'b', 'd', 'c', 'a', 'e', 'f', 'g', 'h', 'o'}; // done in 8
// char finalPuzzle[] = {'b', 'c', 'e', 'g', 'o', 'd', 'f', 'h', 'a'}; // done in 20
// char finalPuzzle[] = {'o', 'h', 'g', 'f', 'e', 'd', 'c', 'b', 'a'}; // done in 28

// dome in 9 steps
char initialPuzzle[] = {'g', 'a', 'e', 'f', 'd', 'b', 'o', 'c', 'h'};
char finalPuzzle[] = {'d', 'o', 'e', 'f', 'g', 'b', 'c', 'a', 'h'};

// done in 10 steps
// char initialPuzzle[] = {'d', 'e', 'c', 'f', 'o', 'g', 'b', 'h', 'a'};
// char finalPuzzle[] = {'d', 'h', 'e', 'a', 'o', 'c', 'f', 'b', 'g'};

// done in 11 steps
// char initialPuzzle[] = {'c', 'g', 'a', 'o', 'f', 'e', 'h', 'd', 'b'};
// char finalPuzzle[] = {'h', 'c', 'g', 'f', 'b', 'a', 'd', 'e', 'o'};

// done in 12 steps
// char initialPuzzle[] = {'b', 'h', 'a', 'f', 'g', 'c', 'd', 'e'};
// char finalPuzzle[] = {'o', 'h', 'a', 'b', 'g', 'c', 'd', 'e', 'f'};

// impossible
// char initialPuzzle[] = {'h', 'a', 'b', 'o', 'd', 'c', 'g', 'f', 'e'};
// char finalPuzzle[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'o'};

// #####################################

// local max depth
int depth = 0;

// variables for the current puzzle state
char currentActionList[MAX_DEPTH];
char currentPuzzle[PUZZLE_LENGTH];
int currentDepth = 0;

// variables
char solutionActionList[MAX_DEPTH];
bool found = false;

// mocking for AKSEN-Functions
#ifdef PC
// definitions for AKSEN functions
void lcd_puts(const char *text)
{
    printf(text);
}
void lcd_cls(void)
{
    printf("\e[1;1H\e[2J");
}
void lcd_ubyte(unsigned char c)
{
    printf("%u", c);
}
void lcd_setxy(unsigned char x, unsigned char y)
{
    printf("\n");
}
void clear_time(void)
{
}
void lcd_ulong(unsigned long l)
{
    printf("%ld", l);
}
void lcd_putchar(char c)
{
    printf("%c", c);
}
void lcd_int(int i)
{
    printf("%d", i);
}
#endif

// initializes the agenda, agendaPointer and agendaPointerOverflow
void init()
{
    unsigned char i;
    unsigned char j;
#ifdef MEM
    addMem(1, INT);
    addMem(2, CHAR);
#endif
    for (i = 0; i < AGENDA_LENGTH; i++)
    {
        for (j = 0; j < MAX_DEPTH; j++)
        {
            agenda[i][j] = 'o';
        }
    }
    agendaPointer = 0;
    agendaPointerOverflow = false;
#ifdef MEM
    remMem(2, CHAR);
    remMem(1, INT);
#endif
}

// fills the current action list with the action list from the top of agenda
void fillCurrentActionList()
{
    unsigned char i;
#ifdef MEM
    addMem(1, INT);
    addMem(1, CHAR);
#endif
    for (i = 0; i < MAX_DEPTH; i++)
    {
        currentActionList[i] = agenda[agendaPointer][i];
    }
#ifdef MEM
    remMem(1, CHAR);
    remMem(1, INT);
#endif
}

// calculates the x and y coordinate from the index
int *getXYFromIndex(int index)
{
    static int coordinates[2];
#ifdef MEM
    addMem(4, INT);
#endif
    coordinates[0] = index % 3;
    coordinates[1] = floorf(index / 3);
#ifdef MEM
    remMem(4, INT);
#endif
    return coordinates;
}

// calculates the index from x and y
int getIndexFromXY(int x, int y)
{
#ifdef MEM
    addMem(3, INT);
    remMem(3, INT);
#endif
    return 3 * y + x;
}

// calculates a new puzzle state from a given puzzle with a given action
char *calcPuzzle(char puzzle[PUZZLE_LENGTH], char action)
{
    static char result[PUZZLE_LENGTH];
    unsigned char i;
    unsigned char j;
    int xy[2];
    int indexStart = -1;
    int *coordinates;
    int indexDest = -1;
#ifdef MEM
    addMem(6, INT);
    addMem(3 + PUZZLE_LENGTH + PUZZLE_LENGTH, CHAR);
#endif
    for (i = 0; i < PUZZLE_LENGTH; i++)
    {
        result[i] = puzzle[i];
    }
    for (i = 0; i < PUZZLE_LENGTH; i++)
    {
        if (puzzle[i] == 'o')
        {
            indexStart = i;
            coordinates = getXYFromIndex(i);
            for (j = 0; j < 2; j++)
            {
                xy[j] = *(coordinates + j);
            }
        }
    }

    switch (action)
    {
    case 'u':
        indexDest = getIndexFromXY(xy[0], xy[1] - 1);
        break;
    case 'd':
        indexDest = getIndexFromXY(xy[0], xy[1] + 1);
        break;
    case 'l':
        indexDest = getIndexFromXY(xy[0] - 1, xy[1]);
        break;
    default: // default is action right
        indexDest = getIndexFromXY(xy[0] + 1, xy[1]);
        break;
    }
    result[indexStart] = result[indexDest];
    result[indexDest] = 'o';
#ifdef MEM
    remMem(6, INT);
    remMem(3 + PUZZLE_LENGTH + PUZZLE_LENGTH, CHAR);
#endif
    return result;
}

// calculates a puzzle state from a given actionList
char *calcPuzzleFromInitWithActionList(char actionList[MAX_DEPTH])
{
    static char calculatedPuzzle[PUZZLE_LENGTH];
    unsigned char i;
#ifdef MEM
    addMem(1, INT);
    addMem(MAX_DEPTH + PUZZLE_LENGTH + 1, CHAR);
#endif
    for (i = 0; i < PUZZLE_LENGTH; i++)
    {
        calculatedPuzzle[i] = initialPuzzle[i];
    }

    for (i = 0; i < MAX_DEPTH; i++)
    {
        if (actionList[i] != 'o')
        {
            char *cPuzzle = calcPuzzle(calculatedPuzzle, actionList[i]);
            unsigned char j;
            for (j = 0; j < PUZZLE_LENGTH; j++)
            {
                calculatedPuzzle[j] = *(cPuzzle + j);
            }
        }
    }
#ifdef MEM
    remMem(1, INT);
    remMem(MAX_DEPTH + PUZZLE_LENGTH + 1, CHAR);
#endif
    return calculatedPuzzle;
}

// calculates the current puzzle state and sets it
void calcAndSetCurrentPuzzle()
{
    char *calculatedPuzzle = calcPuzzleFromInitWithActionList(currentActionList);
    unsigned char i;
#ifdef MEM
    addMem(2, INT);
    addMem(1, CHAR);
#endif
    for (i = 0; i < PUZZLE_LENGTH; i++)
    {
        currentPuzzle[i] = *(calculatedPuzzle + i);
    }
#ifdef MEM
    remMem(2, INT);
    remMem(1, CHAR);
#endif
}

// clears the current agenda and decreases the agendapointer by 1
void clearCurrentAgenda()
{
    unsigned char i;
#ifdef MEM
    addMem(1, INT);
    addMem(1, CHAR);
#endif
    for (i = 0; i < MAX_DEPTH; i++)
    {
        agenda[agendaPointer][i] = 'o';
    }
    agendaPointer = agendaPointer - 1;
#ifdef MEM
    remMem(1, INT);
    remMem(1, CHAR);
#endif
}

// checks if the current puzzle state equals the finalPuzzle
bool checkCurrentIsFinal()
{
    unsigned char i;
#ifdef MEM
    addMem(1, INT);
    addMem(1, CHAR);
#endif
    for (i = 0; i < PUZZLE_LENGTH; i++)
    {
        if (currentPuzzle[i] != finalPuzzle[i])
        {
#ifdef MEM
            remMem(1, INT);
            remMem(1, CHAR);
#endif
            return false;
        }
    }
#ifdef MEM
    remMem(1, INT);
    remMem(1, CHAR);
#endif
    return true;
}

// calculates the length of the current action list
int getLengthOfActionList()
{
    unsigned char i;
#ifdef MEM
    addMem(1, INT);
    addMem(1, CHAR);
#endif
    for (i = 0; i < MAX_DEPTH; i++)
    {
        if (currentActionList[i] == 'o')
        {
#ifdef MEM
            remMem(1, INT);
            remMem(1, CHAR);
#endif
            return i;
        }
    }
#ifdef MEM
    remMem(1, INT);
    remMem(1, CHAR);
#endif
    return MAX_DEPTH;
}

// creates a new node for the agenda
char *createChild(char action)
{
    static char result[MAX_DEPTH];
    unsigned char i;
#ifdef MEM
    addMem(1, INT);
    addMem(2 + MAX_DEPTH, CHAR);
#endif
    for (i = 0; i < MAX_DEPTH; i++)
    {
        result[i] = 'o';
    }
    for (i = 0; i < MAX_DEPTH; i++)
    {
        if (currentActionList[i] != 'o')
        {
            result[i] = currentActionList[i];
        }
        else
        {
            result[i] = action;
            break;
        }
    }
#ifdef MEM
    remMem(1, INT);
    remMem(2 + MAX_DEPTH, CHAR);
#endif
    return result;
}

// creates a new child with a given action and adds it to the agenda
bool createChildAndAddToAgenda(char action)
{
    char *child;
    unsigned char i;
#ifdef MEM
    addMem(2, INT);
    addMem(2, CHAR);
#endif
    agendaPointer = agendaPointer + 1;
    if (agendaPointer >= AGENDA_LENGTH)
    {
#ifdef MEM
        remMem(2, INT);
        remMem(2, CHAR);
#endif
        return false;
    }
    child = createChild(action);
    for (i = 0; i < MAX_DEPTH; i++)
    {
        agenda[agendaPointer][i] = *(child + i);
    }
#ifdef MEM
    remMem(2, INT);
    remMem(2, CHAR);
#endif
    return true;
}

// calculates all possible children and adds them to the agenda
bool calcChildrenAndAddToAgenda()
{
    int xy[2];
    unsigned char i;
    unsigned char j;
    int lastIndex = 0;
    char lastMove = 'x';
    int *coordinates;
#ifdef MEM
    addMem(5, INT);
    addMem(3, CHAR);
#endif
    for (i = 0; i < PUZZLE_LENGTH; i++)
    {
        if (currentPuzzle[i] == 'o')
        {
            coordinates = getXYFromIndex(i); // calculation of the xy-coordinates of the empty field
            for (j = 0; j < 2; j++)
            {
                xy[j] = *(coordinates + j);
            }
        }
    }
    // get last move
    lastIndex = getLengthOfActionList() - 1;
    if (lastIndex >= 0)
    {
        lastMove = currentActionList[lastIndex];
    }
    if (xy[0] < 2 && lastMove != 'l')
    { // movement to the right is possible
        if (!createChildAndAddToAgenda('r'))
        {
#ifdef MEM
            remMem(5, INT);
            remMem(3, CHAR);
#endif
            return false;
        }
    }
    if (xy[0] > 0 && lastMove != 'r')
    { // movement to the left is possible
        if (!createChildAndAddToAgenda('l'))
        {
#ifdef MEM
            remMem(5, INT);
            remMem(3, CHAR);
#endif
            return false;
        }
    }
    if (xy[1] < 2 && lastMove != 'u')
    { // movement down is possible
        if (!createChildAndAddToAgenda('d'))
        {
#ifdef MEM
            remMem(5, INT);
            remMem(3, CHAR);
#endif
            return false;
        }
    }
    if (xy[1] > 0 && lastMove != 'd')
    { // movement up is possible
        if (!createChildAndAddToAgenda('u'))
        {
#ifdef MEM
            remMem(5, INT);
            remMem(3, CHAR);
#endif
            return false;
        }
    }
#ifdef MEM
    remMem(5, INT);
    remMem(3, CHAR);
#endif
    return true; // successfully executed function
}

// calculates the number of inversions and checks if a solution is possible
void checkIfSolutionIsPossible()
{
    unsigned char i;
    unsigned char j;
#ifdef MEM
    addMem(1, INT);
    addMem(2, CHAR);
#endif
    for (i = 0; i < PUZZLE_LENGTH; i++)
    {
        if (initialPuzzle[i] != 'o')
        {
            for (j = i + 1; j < PUZZLE_LENGTH; j++)
            {
                if (initialPuzzle[i] > initialPuzzle[j])
                {
                    inversionNumberInitial += 1;
                }
            }
        }
        if (finalPuzzle[i] != 'o')
        {
            for (j = i + 1; j < PUZZLE_LENGTH; j++)
            {
                if (finalPuzzle[i] > finalPuzzle[j])
                {
                    inversionNumberFinal += 1;
                }
            }
        }
    }

    solutionIsPossible = inversionNumberInitial % 2 == inversionNumberFinal % 2;
#ifdef MEM
    remMem(1, INT);
    remMem(2, CHAR);
#endif
}

void AksenMain(void)
{
    unsigned long time;
    unsigned char i;
    unsigned char actionListLength = 0;
#ifdef MEM
    addMem(1, LONG);
    addMem(2, CHAR);
#endif
    lcd_puts("Start");
    clear_time();
    checkIfSolutionIsPossible();
    if (solutionIsPossible)
    {
        while (!found && depth < MAX_DEPTH)
        {
            init(); // init the agenda
            depth = depth + DEPTH_ITERATOR;
            if (depth > MAX_DEPTH)
            {
                depth = MAX_DEPTH;
            }
#ifndef PC
            lcd_cls();
#endif
            lcd_int(depth);

            while (agendaPointer >= 0 && !agendaPointerOverflow)
            {
                fillCurrentActionList();
                calcAndSetCurrentPuzzle();
                clearCurrentAgenda();

                if (!checkCurrentIsFinal())
                {
                    currentDepth = getLengthOfActionList();
                    if (currentDepth < depth)
                    {
                        agendaPointerOverflow = !calcChildrenAndAddToAgenda();
                    }
                }
                else
                { // solution was found
                    for (i = 0; i < MAX_DEPTH; i++)
                    {
                        solutionActionList[i] = currentActionList[i];
                    }
                    agendaPointer = -1;
                    found = true;
                }
            }
            if (agendaPointerOverflow)
            { // there is an agenda pointer overflow
#ifndef PC
                lcd_cls();
#endif
                lcd_puts("Overflow...");
            }
        }
    }
#ifdef MEM
    remMem(1, LONG);
    remMem(2, CHAR);
#endif
#ifndef PC
    time = akt_time();
    lcd_cls();
    // ### UNCOMMENT TO GET OUTPUT WITH TIME ###
    // lcd_ulong(time);
    // lcd_puts("ms");
    // lcd_setxy(1, 0);
    // if (found)
    // {
    //     lcd_puts("Found: ");
    //     lcd_int(getLengthOfActionList());
    //     lcd_puts(" steps");
    // }
    // else if (!solutionIsPossible)
    // {
    //     lcd_puts("No solution");
    // }
    // else
    // {
    //     lcd_puts("Longer then steps");
    // }
    // #########################################
    // ### COMMENT BLOCK TO DISABLE ACTION LIST OUTPUT ###
    if (found)
    {
        actionListLength = getLengthOfActionList();
        for (i = 0; i < actionListLength; i++)
        {
            lcd_putchar(currentActionList[i]);
            if (i == 15)
            {
                lcd_setxy(1, 0);
            }
        }
    }
    else if (!solutionIsPossible)
    {
        lcd_puts("No solution");
    }
    else
    {
        lcd_puts("Longer then steps");
    }
    // ###################################################
#endif
#ifdef PC
    printf("\n");
    if (found)
    {
        lcd_puts("Found: ");
        lcd_int(getLengthOfActionList());
        lcd_puts(" steps");
        printf("\n");
        for (i = 0; i < getLengthOfActionList(); i++)
        {
            printf("%c", currentActionList[i]);
        }
    }
    else if (!solutionIsPossible)
    {
        lcd_puts("No solution");
    }
    else
    {
        lcd_puts("Longer then steps");
    }
#ifdef MEM
    printf("\n");
    printf("max memory usage: %d Byte", maxMem);
#endif
#endif
#ifndef PC
    while (1)
        ;
#endif
}

#ifdef PC
int main()
{
    AksenMain();
}
#endif