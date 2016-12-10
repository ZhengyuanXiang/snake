#include <wiringPi.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "PCD8544.h"

#define INIT_LEN 8
#define BLOCK_SIZE 2

#define TTY_PATH    "/dev/tty" 
#define STTY_US    "stty raw -echo -F " 
#define STTY_DEF    "stty -raw echo -F " 

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
}

void move(char direct)
{
    Body *tmp; 
    int x, y;
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
}

static char MOVE[10] = {LEFT, LEFT, DOWN, DOWN, DOWN, RIGHT, RIGHT, UP, UP, RIGHT};
//static char MOVE[10] = {LEFT, LEFT, LEFT, LEFT, LEFT, LEFT, LEFT, LEFT, LEFT,LEFT};

static int get_char() 
{
    fd_set rfds;
    struct timeval tv;  
    int ch = -1;   
    FD_ZERO(&rfds);  
    FD_SET(0, &rfds);  
    tv.tv_sec = 0;  
    tv.tv_usec = 10;     
    
    while (1)
    {
        if (select(1, &rfds, NULL, NULL, &tv) > 0)  
        {   
            ch = getchar();
            printf("input= %d", ch);
              
        } 
    }
    
    return ch; 
}

int main (void)
{
    int step = 0;
    if (wiringPiSetup() == -1)
    {
        printf("wiringPi-Error\n");
        exit(1);
    }
    system(STTY_US TTY_PATH); 

    LCDInit(0, 1, 2, 3, 4, 45);
    LCDclear();

    init_snake(40, 10);
    LCDdisplay();

    while(true)
    {

    }
    /*for (; step < 10; step++)
    {
        move(MOVE[step]);
        delay(200);
    }*/
}
