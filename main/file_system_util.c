#include "file_system_util.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "esp_system.h"
#include "sdkconfig.h"

static const char *TAG = "FS";

static wl_handle_t s_wl_handle = WL_INVALID_HANDLE;

const esp_vfs_fat_mount_config_t mount_config = {
    .max_files = 4,
    .format_if_mount_failed = false,
    .allocation_unit_size = CONFIG_WL_SECTOR_SIZE};

void initFS()
{
    esp_err_t err;
    err = esp_vfs_fat_spiflash_mount_rw_wl(BASE_FILE, "storage", &mount_config, &s_wl_handle);

    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to mount FATFS (%s)", esp_err_to_name(err));
        return;
    }
    ESP_LOGI(TAG, "Mount Successfully");
    return;
}

uint8_t WriteFile(const char *filePath, uint8_t *buff, uint32_t offset, uint32_t len)
{

    // initFS();
    FILE *f = fopen(filePath, "wb");
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file for writing");
        // unmountFS();
        return 1;
    }

    int ret = fseek(f, offset, SEEK_SET);

    if (ret != 0)
    {
        ESP_LOGE(TAG, "eror to seek file error %d", ret);
        fclose(f);
        // unmountFS();
        return 1;
    }

    size_t written = fwrite(buff, sizeof(uint8_t), len, f);
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
        return 1;
    }

    size_t written = fread(buff, sizeof(uint8_t), len, f);
    fclose(f);
    if (written == len)
    {
        ESP_LOGI(TAG, "read file successfull");
        // unmountFS();
        return 0;
    }
    ESP_LOGE(TAG, "read file Fail");
    return 1;
}

uint8_t createFile(const char *filePath, uint32_t s)
{
    uint8_t buff[512];
    memset(buff, 0, sizeof(buff));
    uint32_t offset = 0;
    FILE *f = fopen(filePath, "wb");
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return 1;
    }

    while (offset < s)
    {
        if (fseek(f, offset, SEEK_SET) != 0)
        {
            ESP_LOGE(TAG, "eror to seek file");
            break;
        }
        uint32_t len = sizeof(buff) < (s - offset) ? sizeof(buff) : (s - offset);
        size_t written = fwrite(buff, sizeof(uint8_t), len, f);

        if (written != len)
        {
            ESP_LOGE(TAG, "Create file Fail");
            break;
        }
        offset += len;
    }
    fclose(f);
    ESP_LOGI(TAG, "Create file Successfull");
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
    return;
}

void unmountFS()
{
    ESP_ERROR_CHECK(esp_vfs_fat_spiflash_unmount_rw_wl(BASE_FILE, s_wl_handle));
}