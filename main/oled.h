#ifndef OLED_H

#define OLED_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdlib.h>
#include "gc9a01.h"



void clearScreen(); 
void initOLED();

// void showBitmapCenter(); 
void showImageFromFile();

#ifdef __cplusplus
}
#endif

#endif
