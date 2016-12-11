#include <wiringPi.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <time.h>

#include "PCD8544.h"

#define INIT_LEN 8
#define BLOCK_SIZE 2

#define X_MAX (LCDWIDTH - 2)
#define Y_MAX (LCDHEIGHT - 2)
#define X_MIN 2
#define Y_MIN 2

#define GO 0
#define DIE 1

enum Direction
{
    UP = 0,
    LEFT,
    DOWN,
    RIGHT
};

typedef struct tagBody
{
    uint8_t x;
    uint8_t y;
    uint8_t dir;
    uint8_t cnt;
    struct tagBody *pre;
    struct tagBody *next;
}Body;

static Body *head;

static Body *apple;
static char cur_dir = LEFT;

void generate_apple()
{
    uint8_t x;
    uint8_t y;
    Body *tmp = head->next;

    /*while (1)
    {
        srand((unsigned) time(NULL));
        x = (rand() % (X_MAX - 2) + 2) / 2;
        y = (rand() % (Y_MAX - 2) + 2) / 2;
        x *= 2;
        y *= 2;
        tmp = head->next;
        for(; tmp != head; tmp = tmp->next)
        {
            if (x == tmp->x && y == tmp->y)
                break;
        }
        if (tmp == head)
            break;
        
    }*/
        srand((unsigned) time(NULL));
        x = (rand() % (X_MAX - 2) + 2) / 2;
        y = (rand() % (Y_MAX - 2) + 2) / 2;
        x *= 2;
        y *= 2;

    apple = (Body *)malloc(sizeof(Body));
    memset((void *)apple, 0, sizeof(Body));
    apple->x = x;
    apple->y = y;
    LCDfillrect(apple->x, apple->y, BLOCK_SIZE, BLOCK_SIZE, BLACK);
    LCDdisplay();
    printf("apple posx=%u posy=%u", x, y);
    //refresh();
    return;
}

void init_snake(uint8_t PosX, uint8_t PosY)
{
    Body *tmp;
    int i;

    head = (Body *)malloc(sizeof(Body));
    memset((void *)head, 0, sizeof(Body));
    head->next = head;
    head->pre  = head;

    for (i = 0; i < INIT_LEN; i++)
    {
        tmp = (Body *)malloc(sizeof(Body));
        tmp->x = PosX - (i * BLOCK_SIZE);
        tmp->y = PosY;
        tmp->next = head->next;
        tmp->pre  = head;
        tmp->dir  = LEFT;
        head->next->pre = tmp;
        head->next = tmp;
        head->cnt++;
    }
    for (tmp = head->next; tmp != head; tmp = tmp->next)
    {
        LCDfillrect(tmp->x, tmp->y, BLOCK_SIZE, BLOCK_SIZE, BLACK);
    }
    generate_apple();

    LCDdrawrect(0, 0, LCDWIDTH, LCDHEIGHT, BLACK);
    LCDdrawrect(1, 1, LCDWIDTH - 2, LCDHEIGHT - 2, BLACK);
}

int move_snake(int dir)
{
    Body *tmp; 
    char direct = dir;
    int x, y;

    if (dir == -1)
        direct = head->next->dir;
    printw("dir=%d ", direct);
    refresh();

    LCDfillrect(head->pre->x, head->pre->y, BLOCK_SIZE, BLOCK_SIZE, WHITE);
    x = head->next->x;
    y = head->next->y;
    switch (direct)
    {
        case UP:
            y -= BLOCK_SIZE;
            break;
        case DOWN:
            y += BLOCK_SIZE;
            break;
        case LEFT:
            x -= BLOCK_SIZE;
            break;
        case RIGHT:
            x += BLOCK_SIZE;
            break;
    }

    if (x == 0 || x == LCDWIDTH || y == 0 || y == LCDHEIGHT ) 
        return DIE;
    if ((x == apple->x && y == apple->y))
            //(x == (apple->x - BLOCK_SIZE) && y == apple->y) ||
            //(x == (apple->x + BLOCK_SIZE) && y == apple->y) ||
            //(x == apple->x && y == (apple->y + BLOCK_SIZE)) ||
            //(x == apple->x && y == (apple->y - BLOCK_SIZE)) )
    {
        apple->dir = direct;
        apple->next = head->next;
        apple->pre = head;
        apple->next->pre = apple;
        head->next = apple;
        generate_apple();
        return GO;
    }
    
    tmp = head->pre;
    tmp->x = x;
    tmp->y = y;
    tmp->dir = direct;

    head->pre = tmp->pre;
    tmp->pre  = head;
    head->next->pre = tmp;
    tmp->next = head->next;
    head->next = tmp;
    LCDfillrect(head->next->x, head->next->y, BLOCK_SIZE, BLOCK_SIZE, BLACK);
    LCDdisplay();
    cur_dir = tmp->dir;
    return GO;
}

static char MOVE[10] = {LEFT, LEFT, DOWN, DOWN, DOWN, RIGHT, RIGHT, UP, UP, RIGHT};
//static char MOVE[10] = {LEFT, LEFT, LEFT, LEFT, LEFT, LEFT, LEFT, LEFT, LEFT,LEFT};


static int get_direct() 
{
    fd_set rfds;
    struct timeval tv; 

    int ch = -1;   
    FD_ZERO(&rfds);  
    FD_SET(0, &rfds);  
    FD_SET(1, &rfds);  
    FD_SET(2, &rfds);  

    tv.tv_sec = 0;  
    tv.tv_usec = 200000;     
    
    while (1)
    {
        if (select(3, &rfds, NULL, NULL, &tv) > 0)  
        {   
            FD_SET(3, &rfds);  
            ch = getch();
            switch (ch)
            {
                case KEY_DOWN:
                    if (cur_dir == UP)
                        break;
                    return DOWN;
                case KEY_UP:
                    if (cur_dir == DOWN)
                        break;
                    return UP;
                case KEY_LEFT:
                    if (cur_dir == RIGHT)
                        break;
                    return LEFT;
                case KEY_RIGHT:
                    if (cur_dir == LEFT)
                        break;
                    return RIGHT;
                default:
                    break;
            }
        } 
        else
            return -1;
    }
}

int main (void)
{
    int direct = 0;
    if (wiringPiSetup() == -1)
    {
        printf("wiringPi-Error\n");
        exit(1);
    }

    LCDInit(0, 1, 2, 3, 4, 45);
    LCDclear();

    init_snake(40, 10);
    LCDdisplay();

    initscr();
    cbreak();
    keypad(stdscr, TRUE); 
    
    while (1)
    {
        direct = get_direct();
        if (move_snake(direct) != GO)
            break;
    }
    endwin();
    printf("\nYou DIE\n\n");
    return 0;
}
