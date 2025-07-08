#include "Functions.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "driver/uart.h"
#include "driver/i2c.h"
#include "sdkconfig.h"
#include "HD44780.h"

void configure_outputs() {
    gpio_config_t out_conf = {
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };

    out_conf.pin_bit_mask = (1ULL << MOTOR_AIN1);
    gpio_config(&out_conf);

    out_conf.pin_bit_mask = (1ULL << MOTOR_AIN2);
    gpio_config(&out_conf);

    out_conf.pin_bit_mask = (1ULL << MOTOR_BIN1);
    gpio_config(&out_conf);

    out_conf.pin_bit_mask = (1ULL << MOTOR_BIN2);
    gpio_config(&out_conf);

    out_conf.pin_bit_mask = (1ULL << LIGHT_SE);
    gpio_config(&out_conf);

    out_conf.pin_bit_mask = (1ULL << LIGHT_ST);
    gpio_config(&out_conf);

    out_conf.pin_bit_mask = (1ULL << LIGHT_Q2);
    gpio_config(&out_conf);

    out_conf.pin_bit_mask = (1ULL << LIGHT_QC);
    gpio_config(&out_conf);

    out_conf.pin_bit_mask = (1ULL << LIGHT_GR);
    gpio_config(&out_conf);

    out_conf.pin_bit_mask = (1ULL << LIGHT_QT);
    gpio_config(&out_conf);

    out_conf.pin_bit_mask = (1ULL << BUZZER_PIN);
    gpio_config(&out_conf);

    out_conf.pin_bit_mask = (1ULL << GATE_PE);
    gpio_config(&out_conf);
}

void configure_inputs() {
    gpio_config_t in_conf = {
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };

    in_conf.pin_bit_mask = (1ULL << ALARM_PT);
    gpio_config(&in_conf);

    in_conf.pin_bit_mask = (1ULL << ALARM_SE);
    gpio_config(&in_conf);

    in_conf.pin_bit_mask = (1ULL << ALARM_ST);
    gpio_config(&in_conf);

    in_conf.pin_bit_mask = (1ULL << ALARM_QC);
    gpio_config(&in_conf);

    in_conf.pin_bit_mask = (1ULL << ALARM_Q2);
    gpio_config(&in_conf);
}

void configure_uart() {
    uart_config_t uart_conf = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM_1, &uart_conf);
    uart_set_pin(UART_NUM_1, GPIO_NUM_1, GPIO_NUM_3, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM_1, 1024 * 2, 0, 0, NULL, 0);
}
