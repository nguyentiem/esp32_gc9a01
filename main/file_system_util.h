#ifndef FILE_SYSTEM_UTIL_H

#define FILE_SYSTEM_UTIL_H

#ifdef __cplusplus
extern "C"
{
#endif
#include <stdlib.h>
#define ImageFile "/spiffs/image.dat"

void initFS(); 
uint8_t WriteFile(const char *filePath, uint8_t *buff, uint32_t offset, uint32_t len); 
uint8_t ReadFile(const char *filePath, uint8_t *buff , uint32_t offset , uint32_t len); 
void unmountFS(); 
uint8_t createFile(const char *filePath, uint32_t s); 
void checkFile(const char *filePath); 
#ifdef __cplusplus
}
#endif

#endif
