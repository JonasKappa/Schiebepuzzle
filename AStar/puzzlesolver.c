//Autor: J. Kappa
//For running on PC: gcc -o ps puzzlesolver.c -D PC
//For running on PC with memory calculation: gcc -o ps puzzlesolver.c -D PC -D MEM

#include <stdbool.h>
#include <stdlib.h>

#ifndef PC
#include <stub.h>
#endif

#ifdef PC
#include <stdio.h>
#include <time.h>
#endif

#define MAX_DEPTH 12
#define AGENDA_LENGTH 255 // 255 * AGENDA_ENTRY_SIZE = 6630 bytes; We need to check if the AGENDA gets an overflow
#define PUZZLE_LENGTH 9
#define DEPTH_ITERATOR 12
#define AGENDA_ENTRY_SIZE 26 // PUZZLE_LENGTH (9) + depth (1) + blank (1) + action (1) + value (1) + link (1) + actionList (MAX DEPTH)
// #define PUZZLE_INDEX 0 -> doesnt needed because its the first index
#define DEPTH_INDEX 9
#define BLANK_INDEX 10
#define ACTION_INDEX 11
#define VALUE_INDEX 12
#define LINK_INDEX 13
#define AL_INDEX 14

#define CHAR 1
#define INT 2
#define LONG 2
#define BOOLEAN 1

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

// variables for the agenda
unsigned char agenda[AGENDA_LENGTH][AGENDA_ENTRY_SIZE]; // organized as linked list
int agendaPointer;
bool agendaPointerOverflow = false;

unsigned char memoryPositions[AGENDA_LENGTH]; // list with possible (free) agenda Positions, organized as stack
int memoryPositionPointer;                    // points to the last field in array which has an available memory position

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

// calculates the heuristic of a puzzle to a target
// it uses the manhattan distance for each tile and sums them up
unsigned char calcHeuristic(unsigned char *puzzle, unsigned char *fPuzzle)
{
    unsigned char i;
    unsigned char j;
    unsigned char x_i;
    unsigned char y_i;
    unsigned char x_j;
    unsigned char y_j;
    unsigned char heuristic = 0;
#ifdef MEM
    addMem(3, INT);
    addMem(7, CHAR);
#endif
    for (i = 0; i < PUZZLE_LENGTH; i++)
    {
        x_i = i % 3;
        y_i = i / 3;
        for (j = 0; j < PUZZLE_LENGTH; j++)
        {
            if (*(puzzle + i) == *(fPuzzle + j))
            { // chars are the same
                x_j = j % 3;
                y_j = j / 3;
                heuristic += abs(x_i - x_j) + abs(y_i - y_j);
            }
        }
    }
#ifdef MEM
    remMem(3, INT);
    remMem(7, CHAR);
#endif
    return heuristic;
}

// puts a new entry in the agenda
void putInAgenda(unsigned char *puzzle, unsigned char d, unsigned char b, unsigned char a, unsigned char v, unsigned char *al)
{
    unsigned char i;
    int pointer = agendaPointer;
    // links initialized as 255, bc that indicates no link
    int postLink = 255;                                         // the entry to link to from new entry
    int prePointer = 255;                                       // the predecessor of the new entry
    unsigned char pos = memoryPositions[memoryPositionPointer]; // position to insert the new entry
#ifdef MEM
    addMem(6, INT);
    addMem(7, CHAR);
#endif
    memoryPositionPointer--;
    // find position to insert
    while (pointer != 255)
    {
        if (agenda[pointer][VALUE_INDEX] >= v)
        { // the agenda entry at pointer has a greater or equal value then our new entry; new entry needs to be inserted directly before pointer
            postLink = pointer;
            break;
        }
        else
        { // the agenda entry at pointer has a lower value then our new entry; remember pointer as predecessor and move to the next link
            prePointer = pointer;
            pointer = agenda[pointer][LINK_INDEX];
        }
    }
    // insert
    for (i = 0; i < PUZZLE_LENGTH; i++)
    {
        agenda[pos][i] = *(puzzle + i);
    }
    agenda[pos][DEPTH_INDEX] = d;
    agenda[pos][BLANK_INDEX] = b;
    agenda[pos][ACTION_INDEX] = a;
    agenda[pos][VALUE_INDEX] = v;
    agenda[pos][LINK_INDEX] = (unsigned char)postLink;
    for (i = 0; i < d; i++)
    {
        agenda[pos][AL_INDEX + i] = *(al + i);
    }
    if (prePointer == 255)
    { // there is no pre, the new entry is the first
        agendaPointer = pos;
    }
    else
    {
        agenda[prePointer][LINK_INDEX] = pos;
    }
#ifdef MEM
    remMem(6, INT);
    remMem(7, CHAR);
#endif
}

// initializes the agenda and the memory positions
void init()
{
    unsigned char i;
    unsigned char j;
    unsigned char emptyList[MAX_DEPTH];
#ifdef MEM
    addMem(1, INT);
    addMem(3 + MAX_DEPTH, CHAR);
#endif
    agendaPointer = 255;
    for (i = 0; i < AGENDA_LENGTH; i++)
    {
        for (j = 0; j < AGENDA_ENTRY_SIZE; j++)
        {
            agenda[i][j] = 0;
        }
    }
    for (i = 0; i < AGENDA_LENGTH; i++)
    {
        memoryPositions[i] = AGENDA_LENGTH - i - 1;
    }
    memoryPositionPointer = AGENDA_LENGTH - 1;
    for (i = 0; i < MAX_DEPTH; i++)
    {
        emptyList[i] = 0;
    }
    agendaPointerOverflow = false;
    // set first entry in agenda
    // costs are 0 so the value is the heuristic only
    putInAgenda(&initialPuzzle[0], 0, INIT_BLANK_POS, 0, calcHeuristic(&initialPuzzle[0], &finalPuzzle[0]), &emptyList[0]);
#ifdef MEM
    remMem(1, INT);
    remMem(3 + MAX_DEPTH, CHAR);
#endif
}

// checks if the problem is solvable
bool checkIfSolutionIsPossible()
{
    unsigned char i;
    unsigned char inversionNumberInitial = 0;
    unsigned char inversionNumberFinal = 0;
#ifdef MEM
    addMem(1, INT);
    addMem(3, CHAR);
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
#ifdef MEM
    remMem(1, INT);
    remMem(3, CHAR);
#endif
    return (inversionNumberInitial % 2 == inversionNumberFinal % 2);
}

// checks wether the current puzzle is the final one
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
bool createChildAndAddToAgenda(unsigned char *puzzle, unsigned char pDepth, unsigned char pBlank, unsigned char action, unsigned char *al)
{
    unsigned char switchPos = 0;
    unsigned char nPuzzle[PUZZLE_LENGTH];
    unsigned char nAL[MAX_DEPTH];
    unsigned char i;
    unsigned char value;
#ifdef MEM
    addMem(3, INT);
    addMem(6 + PUZZLE_LENGTH + MAX_DEPTH, CHAR);
#endif
    if (memoryPositionPointer < 0)
    {
#ifdef MEM
        remMem(3, INT);
        remMem(6 + PUZZLE_LENGTH + MAX_DEPTH, CHAR);
#endif
        return false; // agendaPointer overflow
    }
    for (i = 0; i < PUZZLE_LENGTH; i++)
    {
        nPuzzle[i] = *(puzzle + i);
    }
    for (i = 0; i < pDepth; i++)
    {
        nAL[i] = *(al + i);
    }
    nAL[pDepth] = action;
    value = pDepth + 1 + calcHeuristic(&nPuzzle[0], &finalPuzzle[0]);
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
    putInAgenda(&nPuzzle[0], pDepth + 1, switchPos, action, value, &nAL[0]);
#ifdef MEM
    remMem(3, INT);
    remMem(6 + PUZZLE_LENGTH + MAX_DEPTH, CHAR);
#endif
    return true;
}

// calculate possible children and adds them to the agenda
bool calcChildrenAndAddToAgenda()
{
    unsigned char i;
    unsigned char puzzle[PUZZLE_LENGTH];
    unsigned char al[MAX_DEPTH];
    unsigned char pDepth = agenda[agendaPointer][DEPTH_INDEX];
    unsigned char pBlank = agenda[agendaPointer][BLANK_INDEX];
    unsigned char pAction = agenda[agendaPointer][ACTION_INDEX];
#ifdef MEM
    addMem(1, INT);
    addMem(4 + PUZZLE_LENGTH + MAX_DEPTH, CHAR);
#endif
    for (i = 0; i < PUZZLE_LENGTH; i++)
    {
        puzzle[i] = agenda[agendaPointer][i];
    }
    for (i = 0; i < pDepth; i++)
    {
        al[i] = agenda[agendaPointer][AL_INDEX + i];
    }
    // set the link free
    memoryPositionPointer++;
    memoryPositions[memoryPositionPointer] = agendaPointer;
    agendaPointer = agenda[agendaPointer][LINK_INDEX];
    // move right is possible
    if (pBlank % 3 != 2 && pAction != 'l')
    {
        if (!createChildAndAddToAgenda(&puzzle[0], pDepth, pBlank, 'r', &al[0]))
        {
#ifdef MEM
            remMem(1, INT);
            remMem(4 + PUZZLE_LENGTH + MAX_DEPTH, CHAR);
#endif
            return false;
        }
    }
    // move left is possible
    if (pBlank % 3 != 0 && pAction != 'r')
    {
        if (!createChildAndAddToAgenda(&puzzle[0], pDepth, pBlank, 'l', &al[0]))
        {
#ifdef MEM
            remMem(1, INT);
            remMem(4 + PUZZLE_LENGTH + MAX_DEPTH, CHAR);
#endif
            return false;
        }
    }
    // move down is possible
    if (pBlank <= 5 && pAction != 'u')
    {
        if (!createChildAndAddToAgenda(&puzzle[0], pDepth, pBlank, 'd', &al[0]))
        {
#ifdef MEM
            remMem(1, INT);
            remMem(4 + PUZZLE_LENGTH + MAX_DEPTH, CHAR);
#endif
            return false;
        }
    }
    // move up is possible
    if (pBlank >= 3 && pAction != 'd')
    {
        if (!createChildAndAddToAgenda(&puzzle[0], pDepth, pBlank, 'u', &al[0]))
        {
#ifdef MEM
            remMem(1, INT);
            remMem(4 + PUZZLE_LENGTH + MAX_DEPTH, CHAR);
#endif
            return false;
        }
    }
#ifdef MEM
    remMem(1, INT);
    remMem(4 + PUZZLE_LENGTH + MAX_DEPTH, CHAR);
#endif
    return true;
}

void AksenMain(void)
{
    unsigned long time;
    unsigned char i = 0;
    bool solutionIsPossible;
#ifdef MEM
    addMem(1, LONG);
    addMem(1, CHAR);
    addMem(1, BOOLEAN);
#endif
    lcd_puts("Start");
#ifndef PC
    lcd_cls();
    clear_time();
#endif
    solutionIsPossible = checkIfSolutionIsPossible();
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
#ifdef PC
            printf("\n");
#endif
#ifndef PC
            lcd_setxy(0, 0);
#endif
            lcd_ubyte(depth);

            while (agendaPointer < 255 && !agendaPointerOverflow && !found)
            {
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
                        memoryPositionPointer++;
                        memoryPositions[memoryPositionPointer] = agendaPointer;
                        agendaPointer = agenda[agendaPointer][LINK_INDEX];
                    }
                }
                else
                {
                    found = true;
                }
            }
            if (agendaPointerOverflow)
            {
#ifndef PC
                lcd_setxy(1, 0);
                lcd_puts("A-P-Overflow");
#endif
#ifdef PC
                printf("A-P-Overflow");
#endif
            }
        }
    }
#ifdef MEM
    remMem(1, LONG);
    remMem(1, CHAR);
    remMem(1, BOOLEAN);
#endif
#ifndef PC
    time = akt_time();
    lcd_cls();
    // ### UNCOMMENT TO GET OUTPUT WITH TIME ###
    lcd_ulong(time);
    lcd_puts("ms");
    lcd_setxy(1, 0);
    if (found)
    {
        lcd_puts("Found: ");
        lcd_int(agenda[agendaPointer][DEPTH_INDEX]);
        lcd_puts(" steps");
    }
    else if (!solutionIsPossible)
    {
        lcd_puts("No solution");
    }
    else
    {
        lcd_puts("Longer then steps");
    }
    // #########################################
    // ### COMMENT BLOCK TO DISABLE ACTION LIST OUTPUT ###
    // if (found)
    // {
    //     for (i = 0; i < agenda[agendaPointer][DEPTH_INDEX]; i++)
    //     {
    //         lcd_putchar(agenda[agendaPointer][AL_INDEX + i]);
    //         if (i == 15)
    //         {
    //             lcd_setxy(1, 0);
    //         }
    //     }
    // }
    // else if (!solutionIsPossible)
    // {
    //     lcd_puts("No solution");
    // }
    // else
    // {
    //     lcd_puts("Longer then steps");
    // }
    // ###################################################
#endif
#ifdef PC
    printf("\n");
    if (found)
    {
        printf("Found: %u steps\n", agenda[agendaPointer][DEPTH_INDEX]);
        for (i = 0; i < agenda[agendaPointer][DEPTH_INDEX]; i++)
        {
            lcd_putchar(agenda[agendaPointer][AL_INDEX + i]);
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
