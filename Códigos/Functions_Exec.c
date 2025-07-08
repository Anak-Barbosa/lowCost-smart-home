#include "Functions.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include <string.h>
#include "driver/i2c.h"
#include "sdkconfig.h"
#include "HD44780.h"

void turn_on_light(const int light_pin) {
    gpio_set_level(light_pin, 1);
    LIGHT_SE_STATE = true;
}

void turn_off_light(const int light_pin) {
    gpio_set_level(light_pin, 0);
    LIGHT_SE_STATE = false;
}

void light_invert(const int code) {
    if(code == 1) {
        LIGHT_SE_STATE = !LIGHT_SE_STATE;
        gpio_set_level(LIGHT_SE, LIGHT_SE_STATE);
    }
    else if(code == 2) {
        LIGHT_QC_STATE = !LIGHT_QC_STATE;
        gpio_set_level(LIGHT_QC, LIGHT_QC_STATE);
    }
    else if(code == 3) {
        LIGHT_ST_STATE = !LIGHT_ST_STATE;
        gpio_set_level(LIGHT_ST, LIGHT_ST_STATE);
    }
    else if(code == 4) {
        LIGHT_GR_STATE = !LIGHT_GR_STATE;
        gpio_set_level(LIGHT_GR, LIGHT_GR_STATE);
    }
    else if(code == 5) {
        LIGHT_Q2_STATE = !LIGHT_Q2_STATE;
        gpio_set_level(LIGHT_Q2, LIGHT_Q2_STATE);
    }
    else if(code == 6) {
        LIGHT_QT_STATE = !LIGHT_QT_STATE;
        gpio_set_level(LIGHT_QT, LIGHT_QT_STATE);
    }
}


void open_gate(const char* gate)
{
    if(strcmp(gate, "EX") == 0) {
        gpio_set_level(MOTOR_AIN1, 0);  
        gpio_set_level(MOTOR_AIN2, 1);  
        vTaskDelay(4100 / portTICK_PERIOD_MS);  
        gpio_set_level(MOTOR_AIN1, 0);  
        gpio_set_level(MOTOR_AIN2, 0);
        MOTOR_EX_STATE = 1;  
    }
    else if(strcmp(gate, "IN") == 0) {
        gpio_set_level(MOTOR_BIN1, 0);  
        gpio_set_level(MOTOR_BIN2, 1);  
        vTaskDelay(1600 / portTICK_PERIOD_MS);  
        gpio_set_level(MOTOR_BIN1, 0);  
        gpio_set_level(MOTOR_BIN2, 0);  
        MOTOR_IN_STATE = 1;
    }
    else if(strcmp(gate, "PE") == 0) {
        gpio_set_level(GATE_PE, 1);
        GATE_PE_STATE = true;
    }
}

void close_gate(const char* gate)
{
    if(strcmp(gate, "EX") == 0) {
        gpio_set_level(MOTOR_AIN1, 1);  
        gpio_set_level(MOTOR_AIN2, 0);  
        vTaskDelay(4100 / portTICK_PERIOD_MS);  
        gpio_set_level(MOTOR_AIN1, 0);  
        gpio_set_level(MOTOR_AIN2, 0);
        MOTOR_EX_STATE = 0;  
    }
    else if(strcmp(gate, "IN") == 0) {
        gpio_set_level(MOTOR_BIN1, 1);  
        gpio_set_level(MOTOR_BIN2, 0);  
        vTaskDelay(1600 / portTICK_PERIOD_MS);  
        gpio_set_level(MOTOR_BIN1, 0);  
        gpio_set_level(MOTOR_BIN2, 0); 
        MOTOR_IN_STATE = 0; 
    }
    else if(strcmp(gate, "PE") == 0) {
        gpio_set_level(GATE_PE, 0);
        GATE_PE_STATE = false;
    }
}

void gate_invert(const int code) {
    if ((code == 1) && MOTOR_EX_STATE) {
        close_gate("EX");
    }
    else if ((code == 1) && !MOTOR_EX_STATE) {
        open_gate("EX");
    }
    else if (code == 2) {
        LIGHT_ST_STATE = !LIGHT_ST_STATE;
        gpio_set_level(GATE_PE,GATE_PE_STATE);
    }
    else if ((code == 3) && MOTOR_EX_STATE) {
        close_gate("IN");
    }
    else if ((code == 3) && !MOTOR_EX_STATE) {
        open_gate("IN");
    }
}

void lcd_show_status(int status) {
    LCD_home();
    LCD_clearScreen();

    if (status == -1) {
        LCD_writeStr("ALARME ARMADO");
        LCD_setCursor(0, 1);          
        LCD_writeStr("   VIOLACAO   ");
    } 
    else if (status == 0) {
        LCD_writeStr("ALARME ARMADO");
        LCD_setCursor(0, 1);
        LCD_writeStr(" SEM VIOLACAO ");
    } 
    else if (status == 1) {
        LCD_writeStr("ALARME DESARM");
        LCD_setCursor(0, 1);
        LCD_writeStr("              ");
    }
}

void trigger_alarm(int state) {
    if (state) {
        if (!ALARM_TRIGGERED && gpio_get_level(ALARM_ST)) {
            ALARM_TRIGGERED= true;
            strcpy(ALARM_SECTOR, "ST");
        }
        else if (!ALARM_TRIGGERED && gpio_get_level(ALARM_SE)) {
            ALARM_TRIGGERED= true;
            strcpy(ALARM_SECTOR, "SE");
        }

        if (ALARM_TRIGGERED) {
            gpio_set_level(BUZZER_PIN, 1); 
            lcd_show_status(-1); 
        } 
        else {
            gpio_set_level(BUZZER_PIN, 0);
            lcd_show_status(0); 
        }

    } 
    else if (!state) {
        ALARM_TRIGGERED= false;
        gpio_set_level(BUZZER_PIN, 0);
        lcd_show_status(1); 
    }
}



void task_alarm_monitor(void* arg)
{
    while (1) {
        if (ALARM_STATUS == 'L') {
            trigger_alarm(1);
        }
        else if (ALARM_STATUS == 'D') {
            trigger_alarm(0);
        }

        vTaskDelay(pdMS_TO_TICKS(100));  
    }
}

void task_turn_on(void* arg)
{
    const room_map_t* room = (const room_map_t*) arg;
    turn_on_light(room->pin);
    if (room->state) {
        *(room->state) = true;
    }
    vTaskDelete(NULL);
}

void task_turn_off(void* arg)
{
    const room_map_t* room = (const room_map_t*) arg;
    turn_off_light(room->pin);
    if (room->state) {
        *(room->state) = false;
    }
    vTaskDelete(NULL);
}


void task_open_gate(void* arg)
{
    const char* cmd = (const char*) arg;
    if ((strstr(cmd, "extern") != NULL) && !MOTOR_EX_STATE) {
        open_gate("EX");
    } 
    else if ((strstr(cmd, "intern") != NULL) && !MOTOR_IN_STATE) {
        open_gate("IN");
    }
    else if ((strstr(cmd, "pedestre") != NULL) && !GATE_PE_STATE) {
        open_gate("PE");
    }
    vTaskDelete(NULL);
}

void task_close_gate(void* arg)
{
    const char* cmd = (const char*) arg;
    if ((strstr(cmd, "extern") != NULL) && MOTOR_EX_STATE) {
        close_gate("EX");
    } 
    else if ((strstr(cmd, "intern") != NULL) && MOTOR_IN_STATE) {
        close_gate("IN");
    }
    else if ((strstr(cmd, "pedestre") != NULL) && GATE_PE_STATE) {
        close_gate("PE");
    }
    vTaskDelete(NULL);
}

void task_turn_on_all(void* arg)
{
    turn_on_light(LIGHT_GR);
    turn_on_light(LIGHT_Q2);
    turn_on_light(LIGHT_QC);
    turn_on_light(LIGHT_QT);
    turn_on_light(LIGHT_SE);
    turn_on_light(LIGHT_ST);
    vTaskDelete(NULL);
}

void task_turn_off_all(void* arg)
{
    turn_off_light(LIGHT_GR);
    turn_off_light(LIGHT_Q2);
    turn_off_light(LIGHT_QC);
    turn_off_light(LIGHT_QT);
    turn_off_light(LIGHT_SE);
    turn_off_light(LIGHT_ST);
    vTaskDelete(NULL);
}

void task_buttons_monitor(void* arg)
{
    const char* cmd = (const char*) arg;

    if (strstr(cmd, "se") != NULL) {
        light_invert(1);
    }
    else if (strstr(cmd, "qc") != NULL) {
        light_invert(2);
    }
    else if (strstr(cmd, "st") != NULL) {
        light_invert(3);
    }
    else if (strstr(cmd, "gr") != NULL) {
        light_invert(4);
    }
    else if (strstr(cmd, "q2") != NULL) {
        light_invert(5);
    }
    else if (strstr(cmd, "qt") != NULL) {
        light_invert(6);
    }
    else if (strstr(cmd, "ex") != NULL) {
        gate_invert(1);
    }
    else if (strstr(cmd, "pe") != NULL) {
        gate_invert(2);
    }
    else if (strstr(cmd, "in") != NULL) {
        gate_invert(3);   
    }
    vTaskDelete(NULL);
}


