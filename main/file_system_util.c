#include "file_system_util.h"
#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_spiffs.h"

static const char *TAG = "FS";

esp_vfs_spiffs_conf_t conf = {
    .base_path = "/spiffs",
    .partition_label = NULL,
    .max_files = 5,
    .format_if_mount_failed = true,
    };

void initFS()
{
    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK)
    {
        if (ret == ESP_FAIL)
        {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        }
        else if (ret == ESP_ERR_NOT_FOUND)
        {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        }
        else
        {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return;
    }

    // #ifdef CONFIG_EXAMPLE_SPIFFS_CHECK_ON_START
    //     ESP_LOGI(TAG, "Performing SPIFFS_check().");
    //     ret = esp_spiffs_check(conf.partition_label);
    //     if (ret != ESP_OK)
    //     {
    //         ESP_LOGE(TAG, "SPIFFS_check() failed (%s)", esp_err_to_name(ret));
    //         return;
    //     }
    //     else
    //     {
    //         ESP_LOGI(TAG, "SPIFFS_check() successful");
    //     }
    // #endif

    // size_t total = 0, used = 0;
    // ret = esp_spiffs_info(conf.partition_label, &total, &used);
    // if (ret != ESP_OK)
    // {
    //     ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s). Formatting...", esp_err_to_name(ret));
    //     esp_spiffs_format(conf.partition_label);
    //     return;
    // }
    // else
    // {
    //     ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    // }

    // #
    //     // Check consistency of reported partiton size info.
    //     if (used > total)
    //     {
    //         ESP_LOGW(TAG, "Number of used bytes cannot be larger than total. Performing SPIFFS_check().");
    //         ret = esp_spiffs_check(conf.partition_label);
    //         // Could be also used to mend broken files, to clean unreferenced pages, etc.
    //         // More info at https://github.com/pellepl/spiffs/wiki/FAQ#powerlosses-contd-when-should-i-run-spiffs_check
    //         if (ret != ESP_OK)
    //         {
    //             ESP_LOGE(TAG, "SPIFFS_check() failed (%s)", esp_err_to_name(ret));
    //             return;
    //         }
    //         else
    //         {
    //             ESP_LOGI(TAG, "SPIFFS_check() successful");
    //         }
    //     }
    return;
}

uint8_t WriteFile(const char *filePath, uint8_t *buff, uint32_t offset, uint32_t len)
{

    // initFS();
    FILE *f = fopen(filePath, "wb+");
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file for writing");
        // unmountFS();
        return 1;
    }

    int ret = fseek(f, offset, SEEK_SET);

    if (ret != 0)
    {
        if (fseek(f, offset, SEEK_SET) == -1)
        {
            ESP_LOGE(TAG, "fseek failed, reopening file");
            fclose(f);
            f = fopen(filePath, "a");
            if (!f)
            {
                ESP_LOGE(TAG, "Error reopening file");
                // unmountFS();
                return 1;
            }
        }
        else
        {
            ESP_LOGE(TAG, "eror to seek file error %d", ret);
            fclose(f);
            // unmountFS();
            return 1;
        }
    }

    size_t written = fwrite(buff, sizeof(uint8_t), len, f);
    fflush(f);
    fclose(f);
    if (written == len)
    {
        ESP_LOGI(TAG, "Write file successfull");
        // unmountFS();
        return 0;
    }
    ESP_LOGE(TAG, "Write file Fail");
    // unmountFS();
    return 1;
}

uint8_t ReadFile(const char *filePath, uint8_t *buff, uint32_t offset, uint32_t len)
{
    // initFS();
    FILE *f = fopen(filePath, "rb");
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file for Reading");
        // unmountFS();
        return 1;
    }

    if (fseek(f, offset, SEEK_SET) != 0)
    {
        ESP_LOGE(TAG, "eror to seek file");
        fclose(f);
        // unmountFS();
        return 1;
    }

    size_t written = fread(buff, sizeof(uint8_t), len, f);
    //   fflush(f);
    fclose(f);
    if (written == len)
    {
        ESP_LOGI(TAG, "read file successfull");
        // unmountFS();
        return 0;
    }
    // unmountFS();
    return 1;
}

uint8_t createFile(const char *filePath, uint32_t s)
{
    // initFS();
    uint8_t buff[512];
    memset(buff, 0, sizeof(buff));
    uint32_t offset = 0;
    FILE *f = fopen(filePath, "wb+");
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file for writing");
        // unmountFS();
        return 1;
    }

    while (offset < s)
    {
        // if (offset != 0)
        // {
        //     if (fseek(f, offset - 1, SEEK_SET) != 0)
        //     {
        //         ESP_LOGE(TAG, "eror to seek file");
        //         fclose(f);
        //         return 1;
        //     }
        // }
        uint32_t len = sizeof(buff) < (s - offset) ? sizeof(buff) : (s - offset);
        size_t written = fwrite(buff, sizeof(uint8_t), len, f);

        if (written != len)
        {
            ESP_LOGE(TAG, "Create file Fail");
            fclose(f);
            // unmountFS();
            return 1;
        }
        offset += len;
    }
    fflush(f);
    fclose(f);
    ESP_LOGI(TAG, "Create file Successfull");
    // unmountFS();
    return 0;
}

void checkFile(const char *filePath)
{
    // initFS();
    struct stat st;
    int ret = stat(filePath, &st);
    if (ret != 0)
    {
        ESP_LOGE(TAG, "Failed to get file size");
        return;
    }
    ESP_LOGI(TAG, "File size: %ld bytes", st.st_size);
    // unmountFS();
    return;
}

void unmountFS()
{
    esp_vfs_spiffs_unregister(conf.partition_label);
}