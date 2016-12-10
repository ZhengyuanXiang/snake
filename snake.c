#include "PCD8544.h"
#include <wiringPi.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int _din = 1;
int _sclk = 0;
int _dc = 2;
int _rst = 4;
int _cs = 3;

int contrast = 45; 

int main (void)
{
    if (wiringPiSetup() == -1)
    {
        printf("wiringPi-Error\n");
        exit(1);
    }

    LCDInit(_sclk, _din, _dc, _cs, _rst, contrast);
    LCDclear();

    LCDfillrect(100, 100, 2, 2, BLACK);
    LCDdisplay();
    delay(111000);
}