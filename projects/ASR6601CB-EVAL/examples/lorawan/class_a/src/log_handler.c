#include "log_handler.h"
#include "tremo_uart.h"
#include "tremo_gpio.h"
#include <stdio.h>
#include <string.h>

// Lokalna implementacija funkcije uart_send_polling
void uart_send_polling(uart_t* uartx, const uint8_t* data, size_t length) {
    for (size_t i = 0; i < length; i++) {
        // Cekajte dok TX FIFO nije spreman
        while (uart_get_flag_status(uartx, UART_FLAG_TX_FIFO_FULL) == SET)
            ;
        // Pošaljite jedan bajt podataka
        uart_send_data(uartx, data[i]);
    }
}

// Inicijalizacija UART-a za logove
void log_init(void) {
    gpio_set_iomux(GPIOB, GPIO_PIN_0, 1);
    gpio_set_iomux(GPIOB, GPIO_PIN_1, 1);

    uart_config_t uart_config;
    uart_config_init(&uart_config);

    uart_config.baudrate = UART_BAUDRATE_115200;
    uart_init(CONFIG_DEBUG_UART, &uart_config);
    uart_cmd(CONFIG_DEBUG_UART, ENABLE);
}

// Funkcija za slanje log poruke
void log_message(const char* message) {
    uart_send_polling(CONFIG_DEBUG_UART, (uint8_t*)message, strlen(message));
}

// Funkcija za slanje formatirane log poruke
void log_message_format(const char* format, ...) {
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    log_message(buffer);
}