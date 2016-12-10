#include <wiringPi.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "PCD8544.h"

#define INIT_LEN 3

enum Direction
{
    UP = 0,
    LEFT,
    RIGHT,
    Down
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

void init_snake(uint8_t PosX, uint8_t PosY, uint8_t BlockSize)
{
    Body *tmp;

    head = (Body *)malloc(sizeof(Body));
    memset((void *)head, 0, sizeof(Body));
    head->next = head;
    head->pre  = head;
    tmp->dir  = LEFT;

    for (int i = 0; i < INIT_LEN; i++)
    {
        tmp = (Body *)malloc(sizeof(Body));
        tmp->x = PosX + (i * BlockSize);
        tmp->y = PosY;
        tmp->next = head->next;
        tmp->pre  = head;
        head->next->pre = tmp;
        head->next = tmp;
        head->cnt++;
    }

    for (tmp = head->next; tmp != head; tmp = tmp->next)
    {
        LCDfillrect(tmp->x, tmp->y, BlockSize, BlockSize, BLACK);
    }
}


int main (void)
{
    if (wiringPiSetup() == -1)
    {
        printf("wiringPi-Error\n");
        exit(1);
    }

    LCDInit(0, 1, 2, 3, 4, 45);
    LCDclear();

    init_snake(10, 10, 2);
    LCDdisplay();

    delay(111000);
}