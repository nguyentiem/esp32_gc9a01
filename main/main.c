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


extern void delay(uint32_t ms);


void app_main(void)
{
     ESP_LOGI(TAG, "Start main app");
    initFS();
    checkFile(IMAGE_FILE_PATH);
    initOLED();
    clearScreen();
    showImageFromFile();

    while (1)
    {

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
