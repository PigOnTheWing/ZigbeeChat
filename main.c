#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "app_uart.h"
#include "app_error.h"
#include "nrf_uart.h"
#include "pca10100.h"
#include "app_util_platform.h"
#include "nrf_error.h"


#define UART_BUF_SIZE_TX 128
#define UART_BUF_SIZE_RX 128
#define CARRIAGE_RETURN 13


void handle_event(app_uart_evt_t *evt) {
    switch (evt->evt_type) {
        case APP_UART_COMMUNICATION_ERROR: {
            APP_ERROR_HANDLER(evt->data.error_communication);
            break;
        }
        case APP_UART_FIFO_ERROR: {
            APP_ERROR_HANDLER(evt->data.error_code);
            break;
        }
        default: {

        }
    }
}

int main() {
    uint8_t ch, msg[UART_BUF_SIZE_RX];
    uint32_t err_code, ch_count, i;

    const app_uart_comm_params_t comm_params = {
            RX_PIN_NUMBER,
            TX_PIN_NUMBER,
            RTS_PIN_NUMBER,
            CTS_PIN_NUMBER,
            APP_UART_FLOW_CONTROL_DISABLED,
            false,
            NRF_UART_BAUDRATE_115200
    };

    APP_UART_FIFO_INIT(&comm_params,
            UART_BUF_SIZE_RX,
            UART_BUF_SIZE_TX,
            handle_event,
            APP_IRQ_PRIORITY_LOW,
            err_code);

    APP_ERROR_CHECK(err_code);

    printf("\r\nWelcome to UART repeater.\r\n");

    while (true) {
        ch_count = 0;

        while (app_uart_put('>') != NRF_SUCCESS);
        while (app_uart_get(&ch) != NRF_SUCCESS);

        do {
            msg[ch_count] = ch;
            ch_count++;
            while(app_uart_put(ch) != NRF_SUCCESS);
            while(app_uart_get(&ch) != NRF_SUCCESS);
        }
        while (ch != CARRIAGE_RETURN && ch_count != UART_BUF_SIZE_RX);

        printf("\r\nYour Message:\r\n");

        for (i = 0; i < ch_count; i++) {
            while(app_uart_put(msg[i]) != NRF_SUCCESS);
        }

        printf("\r\n");
    }
}
