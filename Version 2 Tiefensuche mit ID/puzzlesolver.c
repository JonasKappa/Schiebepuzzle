#include <stdbool.h>
#include <stdlib.h>

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
#define AGENDA_ENTRY_SIZE 12 // PUZZLE_LENGTH (9) + depth (1) + blank (1) + action (1)
// #define PUZZLE_INDEX 0 -> isnt needed cause its 0
#define DEPTH_INDEX 9
#define BLANK_INDEX 10
#define ACTION_INDEX 11

#define CHAR 1
#define INT 2
#define LONG 2

#ifdef MEM
int memoryStack = 0;
int maxMem = 0;

void addMem(int n, int x)
{
    memoryStack = memoryStack + (n * x);
    if (maxMem < memoryStack)
    {
        maxMem = memoryStack;
    }
}

void remMem(int n, int x)
{
    memoryStack = memoryStack - (n * x);
    if (memoryStack < 0)
    {
        printf("memoryStack is less then 0!!!!");
    }
}
#endif

// variables for checking if a solution is possible
unsigned char inversionNumberInitial = 0;
unsigned char inversionNumberFinal = 0;
bool solutionIsPossible = true;

// variables for the agenda
unsigned char agenda[AGENDA_LENGTH][AGENDA_ENTRY_SIZE];
char agendaPointer;
bool agendaPointerOverflow = false;

// ### DIFFERENT PUZZLE COMBINATIONS ###
// #########################
// ### !!! ATTENTION !!! ###
// #########################
// this needs to be modified to the index of the 'o' in the initalPuzzle
#define INIT_BLANK_POS 6
// #########################
// #########################
// #########################
// unsigned char initialPuzzle[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'o'};
// unsigned char finalPuzzle[] = {'b', 'd', 'c', 'a', 'e', 'f', 'g', 'h', 'o'}; // 8
// unsigned char finalPuzzle[] = {'b', 'c', 'e', 'g', 'o', 'd', 'f', 'h', 'a'}; // 20
// unsigned char finalPuzzle[] = {'o', 'h', 'g', 'f', 'e', 'd', 'c', 'b', 'a'}; // done in 28

// dome in 9 steps
unsigned char initialPuzzle[] = {'g', 'a', 'e', 'f', 'd', 'b', 'o', 'c', 'h'};
unsigned char finalPuzzle[] = {'d', 'o', 'e', 'f', 'g', 'b', 'c', 'a', 'h'};

// done in 10 steps
// unsigned char initialPuzzle[] = {'d', 'e', 'c', 'f', 'o', 'g', 'b', 'h', 'a'};
// unsigned char finalPuzzle[] = {'d', 'h', 'e', 'a', 'o', 'c', 'f', 'b', 'g'};

// done in 11 steps
// unsigned char initialPuzzle[] = {'c', 'g', 'a', 'o', 'f', 'e', 'h', 'd', 'b'};
// unsigned char finalPuzzle[] = {'h', 'c', 'g', 'f', 'b', 'a', 'd', 'e', 'o'};

// done in 12 steps
// unsigned char initialPuzzle[] = {'o', 'f', 'd', 'g', 'b', 'h', 'e', 'c', 'a'};
// unsigned char finalPuzzle[] = {'f', 'b', 'd', 'c', 'a', 'e', 'g', 'h', 'o'};

// done in 15 steps
// unsigned char initialPuzzle[] = {'f', 'd', 'b', 'a', 'h', 'g', 'c', 'o', 'e'};
// unsigned char finalPuzzle[] = {'f', 'c', 'b', 'a', 'g', 'e', 'o', 'h', 'd'};

// done in 16 steps
// unsigned char initialPuzzle[] = {'a', 'e', 'g', 'c', 'f', 'd', 'o', 'b', 'h'};
// unsigned char finalPuzzle[] = {'b', 'e', 'd', 'g', 'c', 'f', 'a', 'h', 'o'};

// done in 17 steps
// unsigned char initialPuzzle[] = {'h', 'o', 'd', 'b', 'g', 'f', 'c', 'e', 'a'};
// unsigned char finalPuzzle[] = {'f', 'g', 'o', 'e', 'd', 'h', 'b', 'c', 'a'};

// done in 18 steps
// unsigned char initialPuzzle[] = {'h', 'f', 'd', 'o', 'b', 'e', 'g', 'c', 'a'};
// unsigned char finalPuzzle[] = {'a', 'o', 'f', 'd', 'h', 'b', 'g', 'e', 'c'};

// impossible
// unsigned char initialPuzzle[] = {'h', 'a', 'b', 'o', 'd', 'c', 'g', 'f', 'e'};
// unsigned char finalPuzzle[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'o'};

// #####################################

// the local max depth
unsigned char depth = 0;

// global action list
unsigned char actionList[MAX_DEPTH];

// wether a solution was found or not
bool found = false;

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
#endif

// puts an entry on the agenda
void putInAgenda(unsigned char *puzzle, unsigned char d, unsigned char b, unsigned char a)
{
    unsigned char i;
#ifdef MEM
    addMem(2, INT);
    addMem(4, CHAR);
#endif
    for (i = 0; i < PUZZLE_LENGTH; i++)
    {
        agenda[agendaPointer][i] = *(puzzle + i);
    }
    agenda[agendaPointer][DEPTH_INDEX] = d;
    agenda[agendaPointer][BLANK_INDEX] = b;
    agenda[agendaPointer][ACTION_INDEX] = a;
#ifdef MEM
    remMem(2, INT);
    remMem(4, CHAR);
#endif
}

// initializes the agenda and the actionlist
void init()
{
    unsigned char i;
    unsigned char j;
#ifdef MEM
    addMem(1, INT);
    addMem(2, CHAR);
#endif
    agendaPointer = 0;
    for (i = 0; i < AGENDA_LENGTH; i++)
    {
        for (j = 0; j < AGENDA_ENTRY_SIZE; j++)
        {
            agenda[i][j] = 0;
        }
    }
    for (i = 0; i < MAX_DEPTH; i++)
    {
        actionList[i] = 0;
    }
    agendaPointerOverflow = false;
    // set first entry in agenda
    putInAgenda(&initialPuzzle[0], 0, INIT_BLANK_POS, 0);
#ifdef MEM
    remMem(1, INT);
    remMem(2, CHAR);
#endif
}

// checks wether the problem is solvable or not
void checkIfSolutionIsPossible()
{
    unsigned char i;
#ifdef MEM
    addMem(1, INT);
    addMem(1, CHAR);
#endif
    for (i = 0; i < PUZZLE_LENGTH; i++)
    {
        if (initialPuzzle[i] != 'o')
        {
            unsigned char j;
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
            unsigned char j;
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
    remMem(1, CHAR);
#endif
}

// checks if the current state is equal to the final state
bool checkCurrentIsFinal()
{
    unsigned char i;
#ifdef MEM
    addMem(1, INT);
    addMem(1, CHAR);
#endif
    for (i = 0; i < PUZZLE_LENGTH; i++)
    {
        if (agenda[agendaPointer][i] != finalPuzzle[i])
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

// creates a new child and adds it to the agenda
bool createChildAndAddToAgenda(unsigned char *puzzle, unsigned char pDepth, unsigned char pBlank, unsigned char action)
{
    unsigned char switchPos = 0;
    unsigned char nPuzzle[PUZZLE_LENGTH];
    unsigned char i;
#ifdef MEM
    addMem(2, INT);
    addMem(5 + PUZZLE_LENGTH, CHAR);
#endif
    agendaPointer++;
    if (agendaPointer >= AGENDA_LENGTH)
    {
#ifdef MEM
        remMem(2, INT);
        remMem(5 + PUZZLE_LENGTH, CHAR);
#endif
        return false; // agendaPointer overflow
    }
    for (i = 0; i < PUZZLE_LENGTH; i++)
    {
        nPuzzle[i] = *(puzzle + i);
    }
    switch (action)
    {
    case 'l':
        switchPos = pBlank - 1;
        break;
    case 'r':
        switchPos = pBlank + 1;
        break;
    case 'd':
        switchPos = pBlank + 3;
        break;
    case 'u':
        switchPos = pBlank - 3;
    }
    nPuzzle[pBlank] = nPuzzle[switchPos];
    nPuzzle[switchPos] = 'o';
    putInAgenda(&nPuzzle[0], pDepth + 1, switchPos, action);
#ifdef MEM
    remMem(2, INT);
    remMem(5 + PUZZLE_LENGTH, CHAR);
#endif
    return true;
}

// calculates all possible children and adds them to the agenda
bool calcChildrenAndAddToAgenda()
{
    unsigned char i;
    unsigned char puzzle[PUZZLE_LENGTH];
    unsigned char pDepth = agenda[agendaPointer][DEPTH_INDEX];
    unsigned char pBlank = agenda[agendaPointer][BLANK_INDEX];
    unsigned char pAction = agenda[agendaPointer][ACTION_INDEX];
#ifdef MEM
    addMem(1, INT);
    addMem(PUZZLE_LENGTH + 3, CHAR);
#endif
    for (i = 0; i < PUZZLE_LENGTH; i++)
    {
        puzzle[i] = agenda[agendaPointer][i];
    }
    agendaPointer--;
    // move right is possible
    if (pBlank % 3 != 2 && pAction != 'l')
    {
        if (!createChildAndAddToAgenda(&puzzle[0], pDepth, pBlank, 'r'))
        {
#ifdef MEM
            remMem(1, INT);
            remMem(PUZZLE_LENGTH + 3, CHAR);
#endif
            return false;
        }
    }
    // move left is possible
    if (pBlank % 3 != 0 && pAction != 'r')
    {
        if (!createChildAndAddToAgenda(&puzzle[0], pDepth, pBlank, 'l'))
        {
#ifdef MEM
            remMem(1, INT);
            remMem(PUZZLE_LENGTH + 3, CHAR);
#endif
            return false;
        }
    }
    // move down is possible
    if (pBlank <= 5 && pAction != 'u')
    {
        if (!createChildAndAddToAgenda(&puzzle[0], pDepth, pBlank, 'd'))
        {
#ifdef MEM
            remMem(1, INT);
            remMem(PUZZLE_LENGTH + 3, CHAR);
#endif
            return false;
        }
    }
    // move up is possible
    if (pBlank >= 3 && pAction != 'd')
    {
        if (!createChildAndAddToAgenda(&puzzle[0], pDepth, pBlank, 'u'))
        {
#ifdef MEM
            remMem(1, INT);
            remMem(PUZZLE_LENGTH + 3, CHAR);
#endif
            return false;
        }
    }
#ifdef MEM
    remMem(1, INT);
    remMem(PUZZLE_LENGTH + 3, CHAR);
#endif
    return true;
}

void AksenMain(void)
{
    unsigned long time;
    unsigned char i;
#ifdef MEM
    addMem(1, LONG);
    addMem(1, CHAR);
#endif
    lcd_puts("Start");
#ifndef PC
    clear_time();
#endif
    // #ifdef PC
    //     time = clock();
    // #endif
    checkIfSolutionIsPossible();
    if (solutionIsPossible)
    {
        while (!found && depth < MAX_DEPTH)
        {
            init();
            depth = depth + DEPTH_ITERATOR;
            if (depth > MAX_DEPTH)
            {
                depth = MAX_DEPTH;
            }
#ifndef PC
            lcd_cls();
#endif
#ifdef PC
            printf("\n");
#endif
            lcd_ubyte(depth);

            while (agendaPointer >= 0 && !agendaPointerOverflow && !found)
            {
                // get top of stack
                // add action to action list
                if (agenda[agendaPointer][DEPTH_INDEX] > 0) // if depth is zero then there is no action to add
                {
                    actionList[(agenda[agendaPointer][DEPTH_INDEX] - 1)] = agenda[agendaPointer][ACTION_INDEX];
                }

                // check if puzzle is final
                if (!checkCurrentIsFinal())
                {
                    // check if we can go deeper
                    if (agenda[agendaPointer][DEPTH_INDEX] < depth)
                    {
                        agendaPointerOverflow = !calcChildrenAndAddToAgenda();
                    }
                    else
                    {
                        agendaPointer--;
                    }
                }
                else
                {
                    found = true;
                }
            }
            if (agendaPointerOverflow)
            {
                lcd_cls();
                lcd_puts("A-P-Overflow");
            }
        }
    }
#ifdef MEM
    remMem(1, LONG);
    remMem(1, CHAR);
#endif
#ifndef PC // only for AKSEN, but needs to be specified in gcc
    time = akt_time();
    lcd_cls();
    // ### UNCOMMENT TO GET OUTPUT WITH TIME ###
    // lcd_ulong(time);
    // lcd_puts("ms");
    // lcd_setxy(1, 0);
    // if (found)
    // {
    //     lcd_puts("Found: ");
    //     lcd_int(agenda[agendaPointer][DEPTH_INDEX]);
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
        for (i = 0; i < agenda[agendaPointer][DEPTH_INDEX]; i++)
        {
            lcd_putchar(actionList[i]);
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
        printf("Found: %u steps\n", agenda[agendaPointer][DEPTH_INDEX]);
        for (i = 0; i < agenda[agendaPointer][DEPTH_INDEX]; i++)
        {
            lcd_putchar(actionList[i]);
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
    return 1;
}

#endif
