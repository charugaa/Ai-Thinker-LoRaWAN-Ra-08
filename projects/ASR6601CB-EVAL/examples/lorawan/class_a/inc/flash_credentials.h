#ifndef FLASH_CREDENTIALS_H
#define FLASH_CREDENTIALS_H

#include <stdbool.h>
#include <stdint.h>

// Konstantne za velicine podataka
#define DEVEUI_SIZE 8  // Velicina DEVEUI u bajtovima
#define APPEUI_SIZE 8  // Velicina APPEUI u bajtovima
#define APPKEY_SIZE 16 // Velicina APPKEY u bajtovima
#define SN_SIZE     10 // Velicina serijskog broja

// Deklaracije funkcija za validaciju
bool is_valid_hex(const char* str, size_t length);
bool is_valid_sn(const char* str, size_t max_length);

// Deklaracije funkcija za rad sa flash memorijom
void save_to_flash(const char* deveui, const char* appeui, const char* appkey, const char* sn);
void load_from_flash(char* deveui, char* appeui, char* appkey, char* sn);

// Deklaracije funkcija za rad sa DEVEUI, APPEUI i APPKEY
void flash_credentials_get_deveui(uint8_t* deveui);
void flash_credentials_get_appeui(uint8_t* appeui);
void flash_credentials_get_appkey(uint8_t* appkey);

void flash_credentials_set(const uint8_t* deveui, const uint8_t* appeui, const uint8_t* appkey);

#endif // FLASH_CREDENTIALS_H