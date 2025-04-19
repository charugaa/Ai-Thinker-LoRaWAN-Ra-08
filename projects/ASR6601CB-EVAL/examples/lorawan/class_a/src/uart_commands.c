#include <stdint.h> // Za uint8_t
#include <stdio.h>  // Za sprintf i sscanf
#include <string.h> // Za strncmp
#include "flash_credentials.h" // Za flash_credentials funkcije
#include "log_handler.h" // Za log_message

// Maksimalna velicina DevEUI u bajtovima
#define DEVEUI_SIZE 8

// Funkcija za obradu komandi primljenih preko UART-a
void process_uart_command(const char *command) {
    if (strncmp(command, "GET_DEVEUI", 10) == 0) {
        uint8_t deveui[DEVEUI_SIZE];
        flash_credentials_get_deveui(deveui);

        // Ispis DevEUI preko UART-a
        log_message("DEVEUI: ");
        for (int i = 0; i < DEVEUI_SIZE; i++) {
            char hex[3];
            sprintf(hex, "%02X", deveui[i]); // Pretvaranje bajta u heksadecimalni string
            log_message(hex); // Slanje svakog bajta kao string
        }
        log_message("\r\n");
    } else if (strncmp(command, "SET_DEVEUI", 10) == 0) {
        uint8_t deveui[DEVEUI_SIZE];

        // Parsiranje DevEUI iz komande
        if (sscanf(command + 11, "%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX",
                   &deveui[0], &deveui[1], &deveui[2], &deveui[3],
                   &deveui[4], &deveui[5], &deveui[6], &deveui[7]) == DEVEUI_SIZE) {
            // Cuvanje DevEUI u flash memoriji
            flash_credentials_set(deveui, NULL, NULL);
            log_message("DEVEUI updated.\r\n");
        } else {
            log_message("Error: Invalid DEVEUI format. Expected 16 hex characters.\r\n");
        }
    } else {
        // Nepoznata komanda
        log_message("Unknown command.\r\n");
    }
}