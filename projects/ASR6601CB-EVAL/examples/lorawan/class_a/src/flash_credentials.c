#include <stdio.h>
#include <string.h>
#include "tremo_flash.h"
#include "flash_credentials.h"
#include "log_handler.h"

// Konstantne za velicine podataka
#define DEVEUI_SIZE 16
#define APPEUI_SIZE 16
#define APPKEY_SIZE 32
#define SN_SIZE     10

// Adrese u flash memoriji
#define FLASH_ADDR_DEVEUI  0x0801F000
#define FLASH_ADDR_APPEUI  0x0801F010
#define FLASH_ADDR_APPKEY  0x0801F020
#define FLASH_ADDR_SN      0x0801F040

// Funkcija za validaciju heksadecimalnih stringova
bool is_valid_hex(const char* str, size_t length) {
    if (strlen(str) != length) {
        return false;
    }
    for (size_t i = 0; i < length; i++) {
        if (!((str[i] >= '0' && str[i] <= '9') || (str[i] >= 'A' && str[i] <= 'F') || (str[i] >= 'a' && str[i] <= 'f'))) {
            return false;
        }
    }
    return true;
}

// Funkcija za validaciju serijskog broja
bool is_valid_sn(const char* str, size_t max_length) {
    size_t length = strlen(str);
    if (length > max_length) {
        return false;
    }
    for (size_t i = 0; i < length; i++) {
        if (!((str[i] >= '0' && str[i] <= '9') || (str[i] >= 'A' && str[i] <= 'Z') || (str[i] >= 'a' && str[i] <= 'z'))) {
            return false;
        }
    }
    return true;
}

// Funkcija za cuvanje podataka u flash memoriju
void save_to_flash(const char* deveui, const char* appeui, const char* appkey, const char* sn) {
    if (deveui) {
        flash_program_bytes(FLASH_ADDR_DEVEUI, (uint8_t*)deveui, DEVEUI_SIZE / 2);
    }
    if (appeui) {
        flash_program_bytes(FLASH_ADDR_APPEUI, (uint8_t*)appeui, APPEUI_SIZE / 2);
    }
    if (appkey) {
        flash_program_bytes(FLASH_ADDR_APPKEY, (uint8_t*)appkey, APPKEY_SIZE / 2);
    }
    if (sn) {
        flash_program_bytes(FLASH_ADDR_SN, (uint8_t*)sn, SN_SIZE);
    }
}

// Funkcija za ucitavanje podataka iz flash memorije
void load_from_flash(char* deveui, char* appeui, char* appkey, char* sn) {
    if (deveui) {
        memcpy(deveui, (const char*)FLASH_ADDR_DEVEUI, DEVEUI_SIZE / 2);
    }
    if (appeui) {
        memcpy(appeui, (const char*)FLASH_ADDR_APPEUI, APPEUI_SIZE / 2);
    }
    if (appkey) {
        memcpy(appkey, (const char*)FLASH_ADDR_APPKEY, APPKEY_SIZE / 2);
    }
    if (sn) {
        memcpy(sn, (const char*)FLASH_ADDR_SN, SN_SIZE);
    }
}