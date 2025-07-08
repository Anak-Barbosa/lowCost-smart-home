#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "Functions.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/i2c.h"
#include "sdkconfig.h"
#include "HD44780.h"

char ALARM_STATUS = 'D';
bool ALARM_TRIGGERED = false;
bool MOTOR_EX_STATE = false;
bool MOTOR_IN_STATE = false;

bool LIGHT_SE_STATE = false;
bool LIGHT_ST_STATE = false;
bool LIGHT_Q2_STATE = false;
bool LIGHT_QC_STATE = false;
bool LIGHT_GR_STATE = false;
bool LIGHT_QT_STATE = false;

bool GATE_PE_STATE = false;

char ALARM_SECTOR[] = "XX";

void app_main(void)
{
    configure_outputs();
    configure_inputs();
    configure_uart();
    LCD_init(LCD_ADDR, SDA_PIN, SCL_PIN, LCD_COLS, LCD_ROWS);

    gpio_set_level(MOTOR_AIN1, 0);
    gpio_set_level(MOTOR_AIN2, 0);
    gpio_set_level(MOTOR_BIN1, 0);
    gpio_set_level(MOTOR_BIN2, 0);

    gpio_set_level(BUZZER_PIN, 0);

    xTaskCreate(task_alarm_monitor, "alarm_monitor", 2048, NULL, 5, NULL);

    uint8_t buffer[BUF_SIZE];

    while (1) {
        int len = uart_read_bytes(UART_NUM, buffer, BUF_SIZE - 1, pdMS_TO_TICKS(100));
        if (len > 0) {
            buffer[len] = '\0';  
            char* cmd = (char*) buffer;

            for (int i = 0; cmd[i]; i++) {
                cmd[i] = tolower((unsigned char)cmd[i]);
            }

            if (strstr(cmd, "alarm:l") != NULL) {
                ALARM_STATUS = 'L';
            } 
            else if (strstr(cmd, "alarm:d") != NULL) {
                ALARM_STATUS = 'D';
            } 
            else if (strstr(cmd, "button:") != NULL || strstr(cmd, "gate:") != NULL) {
                xTaskCreate(task_buttons_monitor, "button_monitor", 2048, cmd, 3, NULL);
            }
            else {

                bool acender = strstr(cmd, "acend") != NULL || strstr(cmd, "ligar") != NULL;
                bool apagar  = strstr(cmd, "apag")  != NULL || strstr(cmd, "deslig")  != NULL;
                bool todas = strstr(cmd, "todas")  != NULL;

                const room_map_t* selected_room = NULL;
                for (size_t i = 0; i < sizeof(room_map)/sizeof(room_map[0]); ++i) {
                    if (strstr(cmd, room_map[i].name) != NULL) {
                        selected_room = &room_map[i];
                        break;
                    }
                }

                if (acender && selected_room) {
                    if(todas){
                        xTaskCreate(task_turn_on_all, "turn_on_all", 2048, NULL, 2, NULL);
                    }
                    else {
                        xTaskCreate(task_turn_on, "turn_on", 2048, (void*)selected_room, 2, NULL);
                    }
                } else if (apagar && selected_room) {
                    if (todas) {
                        xTaskCreate(task_turn_off_all, "turn_off_all", 2048, NULL, 2, NULL);
                    }
                    else {
                        xTaskCreate(task_turn_off, "turn_off", 2048, (void*)selected_room, 2, NULL);
                    }
                } else if (strstr(cmd, "abri") != NULL) {
                    xTaskCreate(task_open_gate, "open_gate", 2048, cmd, 2, NULL);
                } else if (strstr(cmd, "fech") != NULL) {
                    xTaskCreate(task_close_gate, "close_gate", 2048, cmd, 2, NULL);
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
