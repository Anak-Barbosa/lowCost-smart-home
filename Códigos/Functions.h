#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "driver/gpio.h"
#include "driver/i2c.h"
#include "HD44780.h"

#define MOTOR_AIN1 GPIO_NUM_27
#define MOTOR_AIN2 GPIO_NUM_5
#define MOTOR_BIN1 GPIO_NUM_25
#define MOTOR_BIN2 GPIO_NUM_26

#define LIGHT_SE GPIO_NUM_17
#define LIGHT_ST GPIO_NUM_4
#define LIGHT_Q2 GPIO_NUM_16
#define LIGHT_QC GPIO_NUM_23
#define LIGHT_GR GPIO_NUM_18
#define LIGHT_QT GPIO_NUM_19

#define GATE_PE GPIO_NUM_15

#define BUZZER_PIN GPIO_NUM_32

extern char ALARM_STATUS;
extern bool ALARM_TRIGGERED;
extern bool MOTOR_EX_STATE;
extern bool MOTOR_IN_STATE;

extern bool LIGHT_SE_STATE;
extern bool LIGHT_ST_STATE;
extern bool LIGHT_Q2_STATE;
extern bool LIGHT_QC_STATE;
extern bool LIGHT_GR_STATE;
extern bool LIGHT_QT_STATE;

extern bool GATE_PE_STATE;

extern char ALARM_SECTOR[];

#define ALARM_PT GPIO_NUM_13
#define ALARM_SE GPIO_NUM_14
#define ALARM_ST GPIO_NUM_33
#define ALARM_QC GPIO_NUM_34
#define ALARM_Q2 GPIO_NUM_35

#define UART_NUM   UART_NUM_1
#define BUF_SIZE   1024

#define LCD_ADDR 0x27
#define SDA_PIN  21
#define SCL_PIN  22
#define LCD_COLS 16
#define LCD_ROWS 2

#define I2C_MASTER_NUM      I2C_NUM_0
#define I2C_MASTER_SCL_IO   22
#define I2C_MASTER_SDA_IO   21
#define I2C_MASTER_FREQ_HZ  100000

typedef struct {
    const char* name;
    gpio_num_t pin;
    bool* state;
} room_map_t;

static const room_map_t room_map[] = {
    {"quarto casal",    LIGHT_QC, &LIGHT_QC_STATE},
    {"quarto de casal", LIGHT_QC, &LIGHT_QC_STATE},
    {"quarto do casal", LIGHT_QC, &LIGHT_QC_STATE},
    {"quarto dois",     LIGHT_Q2, &LIGHT_Q2_STATE},
    {"sala estar",      LIGHT_SE, &LIGHT_SE_STATE},
    {"sala de estar",   LIGHT_SE, &LIGHT_SE_STATE},
    {"sala tv",         LIGHT_ST, &LIGHT_ST_STATE},
    {"sala de tv",      LIGHT_ST, &LIGHT_ST_STATE},
    {"garagem",         LIGHT_GR, &LIGHT_GR_STATE},
    {"quintal",         LIGHT_QT, &LIGHT_QT_STATE},
};

void configure_outputs(void);
void configure_inputs(void);
void configure_uart(void);

void turn_on_light(const int light_pin);
void turn_off_light(const int light_pin);
void light_invert(const int code);
void open_gate(const char* gate);
void close_gate(const char* gate);
void trigger_alarm(int state);
void lcd_show_status(int status);
void gate_invert(const int code);

void task_alarm_monitor(void* arg);
void task_turn_on(void* arg);
void task_turn_off(void* arg);
void task_open_gate(void* arg);
void task_close_gate(void* arg);
void task_turn_off_all(void* arg);
void task_turn_on_all(void* arg);
void task_buttons_monitor(void* arg);

#endif
