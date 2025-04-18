#ifndef FLASH_CREDENTIALS_H
#define FLASH_CREDENTIALS_H

#include <stdbool.h>

// Konstantne za velicine podataka
#define DEVEUI_SIZE 16
#define APPEUI_SIZE 16
#define APPKEY_SIZE 32
#define SN_SIZE     10

// Deklaracije funkcija
bool is_valid_hex(const char* str, size_t length);
bool is_valid_sn(const char* str, size_t max_length);
void save_to_flash(const char* deveui, const char* appeui, const char* appkey, const char* sn);
void load_from_flash(char* deveui, char* appeui, char* appkey, char* sn);

#endif // FLASH_CREDENTIALS_H