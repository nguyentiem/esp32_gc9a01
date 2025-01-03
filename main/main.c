/* SPI Master Half Duplex EEPROM example.

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "esp_timer.h"
#include "oled.h"
#include "file_system_util.h"
/*
 This code demonstrates how to use the SPI master half duplex mode to read/write a AT932C46D EEPROM (8-bit mode).
*/

static const char TAG[] = "main";
static uint8_t buff[2048];

extern void delay(uint32_t ms);

void app_main(void)
{

    initFS();
    checkFile(ImageFile);
    // createFile(ImageFile, 173056);
    //  checkFile(ImageFile);

    // uint16_t ls[] = {240, 240};
    // if (WriteFile(ImageFile,(uint8_t*) ls, 0, sizeof(ls)) == 0)
    // {
    //     uint32_t offset = 0;
    //     uint32_t totalSize = GC9A01_W * GC9A01_H * 3;
    //     while (offset < totalSize)
    //     {
    //         ESP_LOGI(TAG, "Write offset %lu", offset);
    //         uint16_t len = sizeof(buff) < (totalSize - offset) ? sizeof(buff) : (totalSize - offset);
    //         memcpy(&buff[0], &my_image[offset], len);
    //         if (WriteFile(ImageFile, buff, offset + sizeof(ls), len) == 0)
    //         {
    //             offset += len;
    //         }
    //         else
    //         {
    //             break;
    //         }
    //     }
    // }

    ESP_LOGI(TAG, "Write Done");

    initOLED();
    clearScreen();

    DrawBitmapContinue((uint8_t *)my_image, GC9A01_W * GC9A01_H * 3);

    while (1)
    {

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
