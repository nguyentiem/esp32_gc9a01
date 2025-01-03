#ifndef OLED_H

#define OLED_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdlib.h>
#include "gc9a01.h"
#include "output.h"



void clearScreen(); 
void initOLED();

void showBitmapCenter(); 


#ifdef __cplusplus
}
#endif

#endif
