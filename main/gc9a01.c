#include "gc9a01.h"
#include <stdint.h>

#define ORIENTATION 1
#define COLOR_MODE 0

#define NULL '\0'
static oled gc9a01;

// static uint8_t bitMapBuff[MAX_BIT_MAP_BUFF_SIZE];

const command initCommand[] = {
    {0xef, (uint8_t[]){0x00}, 0},
    {0xeb, (uint8_t[]){0x14}, 1},
    {0xfe, (uint8_t[]){0x00}, 0},
    {0xef, (uint8_t[]){0x00}, 0},
    {0xeb, (uint8_t[]){0x14}, 1},
    {0x84, (uint8_t[]){0x40}, 1},
    {0x85, (uint8_t[]){0xff}, 1},
    {0x86, (uint8_t[]){0xff}, 1},
    {0x87, (uint8_t[]){0xff}, 1},
    {0x88, (uint8_t[]){0x0a}, 1},
    {0x89, (uint8_t[]){0x21}, 1},
    {0x8a, (uint8_t[]){0x00}, 1},
    {0x8b, (uint8_t[]){0x80}, 1},
    {0x8c, (uint8_t[]){0x01}, 1},
    {0x8d, (uint8_t[]){0x01}, 1},
    {0x8e, (uint8_t[]){0xff}, 1},
    {0x8f, (uint8_t[]){0xff}, 1},
    {0xb6, (uint8_t[]){0x00, 0x00}, 2},
// rotation
#if ORIENTATION == 0
    {0x36, (uint8_t[]){0x18}, 1},
#elif ORIENTATION == 1
    {0x36, (uint8_t[]){0x28}, 1},
#elif ORIENTATION == 2
    {0x36, (uint8_t[]){0x48}, 1},
#else
    {0x36, (uint8_t[]){0x88}, 1},
#endif

#if COLOR_MODE == 0
    {LCD_CMD_COLMOD, (uint8_t[]){0x06}, 1},
#elif COLOR_MODE == 1
    {LCD_CMD_COLMOD, (uint8_t[]){0x05}, 1}, // 16 bit
#elif COLOR_MODE == 2
    {LCD_CMD_COLMOD, (uint8_t[]){0x03}, 1}, // 12 bit

#endif
    {0x90, (uint8_t[]){0x08, 0x08, 0x08, 0x08}, 4},
    {0xBD, (uint8_t[]){0x06}, 1},
    {0xBC, (uint8_t[]){0x00}, 1},
    {0xff, (uint8_t[]){0x60, 0x01, 0x04}, 3},
    {0xC3, (uint8_t[]){0x13}, 1},
    {0xC4, (uint8_t[]){0x13}, 1},
    {0xC9, (uint8_t[]){0x22}, 1},
    {0xbe, (uint8_t[]){0x11}, 1},
    {0xe1, (uint8_t[]){0x10, 0x0e}, 2},
    {0xdf, (uint8_t[]){0x21, 0x0c, 0x02}, 3},
    // Set gamma
    {0xF0, (uint8_t[]){0x45, 0x09, 0x08, 0x08, 0x26, 0x2a}, 6},
    {0xF1, (uint8_t[]){0x43, 0x70, 0x72, 0x36, 0x37, 0x6f}, 6},
    {0xF2, (uint8_t[]){0x45, 0x09, 0x08, 0x08, 0x26, 0x2a}, 6},
    {0xF3, (uint8_t[]){0x43, 0x70, 0x72, 0x36, 0x37, 0x6f}, 6},
    {0xed, (uint8_t[]){0x1b, 0x0b}, 2},
    {0xae, (uint8_t[]){0x77}, 1},
    {0xcd, (uint8_t[]){0x63}, 1},
    {0x70, (uint8_t[]){0x07, 0x07, 0x04, 0x0e, 0x0f, 0x09, 0x07, 0x08, 0x03}, 9},
    {0xE8, (uint8_t[]){0x34}, 1}, // 4 dot inversion
    // {0x60, (uint8_t[]){0x38, 0x0b, 0x6D, 0x6D, 0x39, 0xf0, 0x6D, 0x6D}, 8},
    // {0x61, (uint8_t[]){0x38, 0xf4, 0x6D, 0x6D, 0x38, 0xf7, 0x6D, 0x6D}, 8},
    {0x62, (uint8_t[]){0x18, 0x0D, 0x71, 0xED, 0x70, 0x70, 0x18, 0x0F, 0x71, 0xEF, 0x70, 0x70}, 12}, // *
    {0x63, (uint8_t[]){0x18, 0x11, 0x71, 0xF1, 0x70, 0x70, 0x18, 0x13, 0x71, 0xF3, 0x70, 0x70}, 12}, ///*
    {0x64, (uint8_t[]){0x28, 0x29, 0xF1, 0x01, 0xF1, 0x00, 0x07}, 7},
    {0x66, (uint8_t[]){0x3C, 0x00, 0xCD, 0x67, 0x45, 0x45, 0x10, 0x00, 0x00, 0x00}, 10},
    {0x67, (uint8_t[]){0x00, 0x3C, 0x00, 0x00, 0x00, 0x01, 0x54, 0x10, 0x32, 0x98}, 10},
    {0x74, (uint8_t[]){0x10, 0x85, 0x80, 0x00, 0x00, 0x4E, 0x00}, 7}, //*
    {0x98, (uint8_t[]){0x3e, 0x07}, 2},
    {0x35, (uint8_t[]){0x00}, 0},
    {0x21, (uint8_t[]){0x00}, 0},
    // {0x99, (uint8_t[]){0x3e, 0x07}, 2},
};

static void sendData(uint8_t *buff, uint32_t len)
{
    if (len > 0 && buff != NULL)
    {
        gc9a01.setPin(gc9a01.DC, 1);
        gc9a01.setPin(gc9a01.CS, 0);
        gc9a01.send(buff, len);
        gc9a01.setPin(gc9a01.CS, 1);
        gc9a01.setPin(gc9a01.DC, 0);
    }
    return;
}

static void sendCommand(uint8_t command, uint8_t *buff, uint32_t len)
{

    gc9a01.setPin(gc9a01.CS, 0);
    gc9a01.send(&command, 1);
    gc9a01.setPin(gc9a01.CS, 1);
    sendData(buff, len);
   

}

// static void GC9A01_write_command(uint8_t cmd)
// {
    
//     // gc9a01.setPin(gc9a01.CS, 0);
//     // gc9a01.send(&cmd, 1);
//     // gc9a01.setPin(gc9a01.CS, 1);
//    sendCommand(cmd, NULL, 0); 
// }

// static void GC9A01_write_data(uint8_t *data, uint32_t len)
// {
//     // gc9a01.setPin(gc9a01.DC, 1);
//     // gc9a01.setPin(gc9a01.CS, 0);
//     // gc9a01.send(data, len);
//     // gc9a01.setPin(gc9a01.CS, 1);
//     // gc9a01.setPin(gc9a01.DC, 0);
//    sendData(data,len); 
// }


// static inline void GC9A01_write_byte(uint8_t val) {
//     GC9A01_write_data(&val, sizeof(val));
// }

// static void sendCommand(uint8_t command, uint8_t *buff, uint32_t len)
// {

//     GC9A01_write_command(command);
//     GC9A01_write_data(buff,len);

// }


 bool gc9a01_reset(bool hardReset)
{
    if (hardReset && gc9a01.RST != 0)
    {
        gc9a01.setPin(gc9a01.RST, 1);
        gc9a01.delay(10);
        gc9a01.setPin(gc9a01.RST, 0);
        gc9a01.delay(10);
    }
    else
    {
        sendCommand(LCD_CMD_SWRESET, NULL, 0);
        gc9a01.delay(20);
    }
    return true;
}

bool lcdInit(configPin cfgPin, setPin setPin, delayms dl_ms, spiSend snd, uint8_t cs, uint8_t rst, uint8_t bl, uint8_t dc)
{
    if (!cfgPin || !setPin || !dl_ms || !snd || !cs)
    {
        return false;
    }
    gc9a01.delay = dl_ms;
    gc9a01.configPin = cfgPin;
    gc9a01.send = snd;
    gc9a01.BL = bl;
    gc9a01.setPin = setPin;
    gc9a01.RST = rst;
    gc9a01.CS = cs;
    gc9a01.DC = dc;

    if (gc9a01.configPin && gc9a01.BL)
    {
        gc9a01.configPin(gc9a01.BL);
    }

    if (gc9a01.configPin && gc9a01.RST)
    {
        gc9a01.configPin(gc9a01.RST);
    }

    if (gc9a01.configPin && gc9a01.CS)
    {
        gc9a01.configPin(gc9a01.CS);
    }

    if (gc9a01.configPin && gc9a01.DC)
    {
        gc9a01.configPin(gc9a01.DC);
    }
    // sendCommand(LCD_CMD_SLPOUT, NULL, 0);
   gc9a01.setPin(gc9a01.DC, 0);
     gc9a01.delay(1000);

    gc9a01.setPin(gc9a01.CS, 1);
    gc9a01.delay(5);

    gc9a01.setPin(gc9a01.RST, 0);
    gc9a01.delay(10);
    gc9a01.setPin(gc9a01.RST, 1);

    gc9a01.delay(120);

  
    int numCmd = sizeof(initCommand) / sizeof(command);
    for (int i = 0; i < numCmd; i++)
    {
        // GC9A01_write_command(initCommand[i].command);
        // if(initCommand[i].paramLen > 0){
        //    for(int j =0 ; j< initCommand[i].paramLen; j++){
        //     GC9A01_write_byte(*((uint8_t*)((uint8_t*)initCommand[i].param +j)));
        //    }
        // }
        sendCommand(initCommand[i].command, initCommand[i].param, initCommand[i].paramLen);
    }
    sendCommand(LCD_CMD_SLPOUT,NULL, 0);
    gc9a01.delay(120);
    sendCommand(LCD_CMD_DISPON,NULL, 0);
    gc9a01.delay(20);
    return true;
}

void Display(bool enable)
{
    if (enable)
    {
        sendCommand(LCD_CMD_DISPON, NULL ,0);
    }
    else
    {
        sendCommand(LCD_CMD_DISPOFF, NULL ,0);
    }
}

void DrawBitmapContinue(uint8_t *bitMap, uint32_t len)
{
    uint32_t offset = 0;
    uint32_t sendByte = len > MAX_BIT_MAP_BUFF_SIZE ? MAX_BIT_MAP_BUFF_SIZE : len;
    sendCommand(LCD_CMD_RAMWR, &bitMap[offset], sendByte);
    len -= sendByte;
    offset += sendByte;
    while (len > 0)
    {
        sendByte = len > MAX_BIT_MAP_BUFF_SIZE ? MAX_BIT_MAP_BUFF_SIZE : len;
        sendCommand(LCD_CMD_RAMWRC, &bitMap[offset], sendByte);
        len -= sendByte;
        offset += sendByte;
    }
    return;
}

void setFrame(int x_start, int y_start, int x_end, int y_end)
{
    if ((x_start > x_end) && (y_start > y_end))
    {
        // invalid position
        return;
    }
    sendCommand(LCD_CMD_CASET, (uint8_t[]){
                                   (x_start >> 8) & 0xFF,
                                   x_start & 0xFF,
                                   ((x_end - 1) >> 8) & 0xFF,
                                   (x_end - 1) & 0xFF,
                               },
                4);

    sendCommand(LCD_CMD_RASET, (uint8_t[]){
                                   (y_start >> 8) & 0xFF,
                                   y_start & 0xFF,
                                   ((y_end - 1) >> 8) & 0xFF,
                                   (y_end - 1) & 0xFF,
                               },
                4);
}

void DrawBitmap(uint8_t *bitMap, uint32_t len, uint8_t flag)
{
    if (flag == 0)
    { // set new frame
        sendCommand(LCD_CMD_RAMWR, &bitMap[0], len);
    }
    else
    { // continue frame
        sendCommand(LCD_CMD_RAMWRC, &bitMap[0], len);
    }

    return;
}



void GC9A01_write(uint8_t *data, uint32_t len) {
    sendCommand(LCD_CMD_RAMWR,data, len );
    // GC9A01_write_command(LCD_CMD_RAMWR);
    // GC9A01_write_data(data, len);
}

void GC9A01_write_continue(uint8_t *data, uint32_t len) {
    sendCommand(LCD_CMD_RAMWRC,data, len );
    // GC9A01_write_command(LCD_CMD_RAMWRC);
    // GC9A01_write_data(data, len);
}