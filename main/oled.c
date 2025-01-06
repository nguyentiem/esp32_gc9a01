#include "oled.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "gc9a01.h"
#include "esp_err.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "esp_timer.h"
#include <stdint.h>
#include "file_system_util.h"

#define LCD HSPI_HOST
#define PIN_NUM_MISO 19
#define PIN_NUM_MOSI 23
#define PIN_NUM_CLK 18
#define PIN_CS 2
#define PIN_RST 4
#define PIN_DC 15
#define ROW_PER_BUFF 1
#define BUFF_SIZE (GC9A01_W * 3 * ROW_PER_BUFF) // 240 pixel , 3bytes R,G,B , 1 row/ buff

static const char *TAG = "OLED";

static uint8_t buff[BUFF_SIZE];
static spi_device_handle_t spi;
static volatile uint8_t isShowing = false;

void configOLEDPin(uint32_t pin)
{
    ESP_LOGI(TAG, "config pin %lu", pin);
    gpio_config_t io_conf = {0};
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = 1ULL << pin;
    bool flag = false;
    if (gpio_config(&io_conf) != ESP_OK)
    {
        ESP_LOGI(TAG, "Error config pin %lu", pin);
        flag = true;
    }
    while (flag)
        ;
    return;
}

void delay_us(uint64_t delay_time_us)
{
    uint64_t start_time = esp_timer_get_time();
    while ((esp_timer_get_time() - start_time) < delay_time_us)
    {
    }
}

void setPinLevel(uint32_t pin, uint32_t level)
{
    gpio_set_level(pin, level);
}

void delay(uint32_t ms)
{
    ESP_LOGI(TAG, "Delay %lu", ms);
    vTaskDelay(pdMS_TO_TICKS(ms));
}

void sendSPI(uint8_t *buff, uint32_t len)
{
    spi_device_acquire_bus(spi, portMAX_DELAY);
    //  ESP_LOGI(TAG, "sendSPI %lu", len);
    spi_transaction_t t = {
        .tx_buffer = buff,
        .flags = 0,
        // .flags = SPI_TRANS_CS_KEEP_ACTIVE,
        .length = len * 8,
    };
    spi_device_polling_transmit(spi, &t);
    spi_device_release_bus(spi);
}

static void SPIInit()
{
    esp_err_t ret;
    spi_bus_config_t buscfg = {
        .miso_io_num = PIN_NUM_MISO,
        .mosi_io_num = PIN_NUM_MOSI,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = MAX_BIT_MAP_BUFF_SIZE,
    };
    // Initialize the SPI bus
    ret = spi_bus_initialize(LCD, &buscfg, SPI_DMA_CH_AUTO);
    ESP_ERROR_CHECK(ret);
    ESP_LOGI(TAG, "Initializing device...");
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 1 * 1000 * 1000,
        .mode = 0, // SPI mode 0
        .queue_size = 10,
        .dummy_bits = 0,
        .spics_io_num = -1,
        .flags = SPI_DEVICE_HALFDUPLEX,
    };
    spi_bus_add_device(LCD, &devcfg, &spi);
}

void clearScreen()
{
    uint32_t idx = 0;
    memset(buff, 0, sizeof(buff));
    while (idx < (GC9A01_W * GC9A01_H * 3))
    {

        if (idx == 0)
        {
            GC9A01_write(buff, sizeof(buff));
        }
        else
        {
            GC9A01_write_continue(buff, sizeof(buff));
        }
        idx += sizeof(buff);
    }
}

// void showBitmapCenter()
// {
//     uint32_t count = 0;
//     uint8_t color[3];
//     for (int x = 0; x < GC9A01_W; x++)
//     {
//         for (int y = 0; y < GC9A01_H; y++)
//         {
//             if (x >= GC9A01_W / 2 - IMAGE_W / 2 && x <= GC9A01_W / 2 + IMAGE_W / 2 && y >= GC9A01_H / 2 - IMAGE_H / 2 && y <= GC9A01_H / 2 + IMAGE_H / 2)
//             {
//                 color[0] = my_image[count++];
//                 color[1] = my_image[count++];
//                 color[2] = my_image[count++];
//             }
//             else
//             {
//                 color[0] = 0;
//                 color[1] = 0;
//                 color[2] = 0;
//             }
//
//             if (x == 0 && y == 0)
//             {
//                 GC9A01_write(color, sizeof(color));
//             }
//             else
//             {
//                 GC9A01_write_continue(color, sizeof(color));
//             }
//         }
//     }
// }

// void copyImageFileToFlash(){
//     uint32_t offset = 0;
//     uint32_t totalSize = 172800;
//     initFS();
//     checkFile(IMAGE_FILE_PATH);
//
//     FILE *f = fopen(IMAGE_FILE_PATH, "wb");
//     if (f == NULL)
//     {
//         ESP_LOGE(TAG, "Failed to open file for writing");
//         // unmountFS();
//         return;
//     }
//
//     uint16_t ls[] = {240, 240};
//
//     int ret = fseek(f, offset, SEEK_SET);
//     if (ret != 0)
//     {
//         ESP_LOGE(TAG, "eror to seek file error %d", ret);
//         goto exit;
//     }
//
//     size_t written = fwrite(ls, sizeof(uint8_t), sizeof(ls), f);
//     if (written != sizeof(ls))
//     {
//         ESP_LOGE(TAG, "Write file Fail");
//         goto exit;
//     }
//
//     while (offset < totalSize)
//     {
//
//         uint16_t len = sizeof(buff) < (totalSize - offset) ? sizeof(buff) : (totalSize - offset);
//         memcpy(&buff[0], &my_image[offset], len);
//         ESP_LOGI(TAG, "Write %u bytes at %lu", len, (unsigned long)(offset+sizeof(ls)));
//         ret = fseek(f, offset + sizeof(ls), SEEK_SET);
//         if (ret != 0)
//         {
//             ESP_LOGE(TAG, "eror to seek file error %d", ret);
//             break;
//         }
//
//         written = fwrite(buff, sizeof(uint8_t), len, f);
//         if (written != len)
//         {
//             ESP_LOGE(TAG, "Write file Fail");
//            break;
//         }
//         offset += len;
//     }
//
// exit:
//     fclose(f);
//     unmountFS();
// }

void initOLED()
{
    SPIInit();
    lcdInit(configOLEDPin, setPinLevel, delay, sendSPI, PIN_CS, PIN_RST, 0, PIN_DC);
    setFrame(0, 0, GC9A01_W, GC9A01_H);
}

void showImageFromFile()
{
    isShowing = 1; 
    uint16_t len[2]; // 0 is width and 1 is height
    ReadFile(IMAGE_FILE_PATH, (uint8_t *)&len[0], 0, sizeof(len));
    uint32_t totalLen = len[0] * len[1] * 3;
    if (totalLen > (GC9A01_W * GC9A01_H * 3))
    {
        totalLen = (GC9A01_W * GC9A01_H * 3);
    }
    ESP_LOGI(TAG, "Image size %lu", totalLen);
    uint32_t offset = 0;
    uint16_t heightCount = 0;
    uint8_t ret;
    uint16_t rLen;
    uint8_t flag = 0;
    while (offset < totalLen)
    {
        memset(buff, 0, sizeof(buff));
        // rLen = (totalLen - offset) > sizeof(buff) ? sizeof(buff) : (totalLen - offset);
        if (len[1] <= GC9A01_H && (heightCount >= (GC9A01_H - len[1]) / 2) && (heightCount <= (GC9A01_H + len[1]) / 2))
        {
            if (len[0] > GC9A01_W)
            {                                       // read 1 row fit with oled
                rLen = GC9A01_W * 3 * ROW_PER_BUFF; // one row
                ret = ReadFile(IMAGE_FILE_PATH, buff, offset + sizeof(len), rLen);
                  ESP_LOGI(TAG, "W image > W Screen, Read  %u and offset +  %u", rLen, len[0] * 3);
                flag = 1;
                // rLen = len[0] * 3;
            }
            else
            {                                     //<=
                rLen = len[0] * 3 * ROW_PER_BUFF; // image W, 3 color and 1 row
                ret = ReadFile(IMAGE_FILE_PATH, &buff[(GC9A01_W - len[0]) * 3], offset + sizeof(len), rLen);
                ESP_LOGI(TAG, "W image <= W Screen, Read  %u and offset + %u", rLen, rLen);
                rLen = GC9A01_W * 3 * ROW_PER_BUFF;
                flag = 2;
            }
        }
        else
        {
            ret = 0;
            rLen = GC9A01_W * 3 * ROW_PER_BUFF;
             ESP_LOGI(TAG, "H Image > H Screen , Read  %u and offset + %u", rLen, rLen);
            flag = 3;
        }
        if (ret == 0)
        {
            if (offset == 0)
            {
                GC9A01_write(buff, rLen);
            }
            else
            {
                GC9A01_write_continue(buff, rLen);
            }
            if (flag == 1)
            {
                offset += len[0] * 3;
            }
            else
            {
                offset += rLen;
            }

            heightCount+=ROW_PER_BUFF;
        }
        else
        {
            ESP_LOGE(TAG, "Read file image fail");
            break;
        }
        if (heightCount >= GC9A01_H)
        {
             ESP_LOGI(TAG, "heightCount >= GC9A01_H, break");
            
            break;
        }
    }
    isShowing = 0; 
    return;
}