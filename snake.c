#include <wiringPi.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "PCD8544.h"

#define INIT_LEN 8
#define BLOCK_SIZE 2

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
        printf("tmp x=%d y=%d\n", tmp->x, tmp->y);
        LCDfillrect(tmp->x, tmp->y, BLOCK_SIZE, BLOCK_SIZE, BLACK);
    }
}

void move(char direct)
{
    Body *tmp; 
    int x, y;
    printf("white x=%d y=%d\n", head->pre->x, head->pre->y);
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
    printf("x=%d y=%d\n", head->next->x, head->next->y); 
    LCDfillrect(head->next->x, head->next->y, BLOCK_SIZE, BLOCK_SIZE, BLACK);
    LCDdisplay();
}

static char MOVE[10] = {LEFT, LEFT, DOWN, DOWN, DOWN, RIGHT, RIGHT, UP, UP, RIGHT};
//static char MOVE[10] = {LEFT, LEFT, LEFT, LEFT, LEFT, LEFT, LEFT, LEFT, LEFT,LEFT};

int main (void)
{
    int step = 0;
    if (wiringPiSetup() == -1)
    {
        printf("wiringPi-Error\n");
        exit(1);
    }

    LCDInit(0, 1, 2, 3, 4, 45);
    LCDclear();

    init_snake(40, 10);
    LCDdisplay();
    for (; step < 10; step++)
    {
        move(MOVE[step]);
        delay(200);
    }
}
