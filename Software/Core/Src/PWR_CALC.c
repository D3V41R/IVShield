#include "main.h"
#include "stdio.h"
#include "cmsis_os.h"
#include "stdint.h"
#include "stm32f4xx_hal.h"
#include "math.h"
#include "stdbool.h"
#include "string.h"

extern UART_HandleTypeDef huart2;
extern ADC_HandleTypeDef hadc1;
extern I2C_HandleTypeDef hi2c1;

uint16_t adcBuffer[12];

float Pwr = 0;
float ApparentPwr = 0;
float VRms = 0;
float IRms = 0;
float pf = 0;
float frequency = 0;
float period = 0;
bool acPresent = false;
bool relayActive = false;

#define LCD_ADDR (0x27 << 1)

void calculatePwr(void);
void CrossDetection(void);

int mainn(void){
    HAL_ADC_Start(&hadc1);
    Lcd_Init();

    while(1){
        for(int i = 0; i < 12; i++){
            HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
            adcBuffer[i] = HAL_ADC_GetValue(&hadc1);
        }

        calculatePwr();
        CrossDetection();

        if(VRms <= 85.0f){
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
            relayActive = true;
        } else {
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
            relayActive = false;
        }

        Lcd_Update();
        HAL_Delay(25);
    }

    return 0;
}

void Lcd_Update(void){
    char line1[17];
    char line2[17];

    // Line 1: Voltage and frequency
    sprintf(line1, sizeof(line1), "V:%.1fV F:%.1fHz", VRms, frequency);

    // Line 2: Source and AC status
    if(relayActive){
        snprintf(line2, sizeof(line2), "SRC:BACKUP %s", acPresent ? "AC:ON" : "AC:OFF");
    } else {
        snprintf(line2, sizeof(line2), "SRC:GRID   %s", acPresent ? "AC:ON" : "AC:OFF");
    }

    Lcd_SetCursor(0, 0);
    Lcd_SendString(line1);
    Lcd_SetCursor(1, 0);
    Lcd_SendString(line2);
}

void calculatePwr(void){
    Pwr = 0;
    VRms = 0;
    IRms = 0;

    for(int i = 0; i < 12; i++){
        float voltage = ((float)adcBuffer[i] / 4095.0f) * 3.3f;
        float current = 20.0f;

        Pwr += voltage * current;
        VRms += voltage * voltage;
        IRms += current * current;
    }

    Pwr = Pwr / 12.0f;
    VRms = sqrtf(VRms / 12.0f);
    IRms = sqrtf(IRms / 12.0f);

    ApparentPwr = VRms * IRms;
    pf = Pwr / ApparentPwr;
}

void CrossDetection(void){
    static uint32_t lastTime = 0;
    uint32_t now = HAL_GetTick();
    uint32_t periodMs = now - lastTime;

    if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) == GPIO_PIN_SET){
        if(periodMs > 0 && periodMs < 100){
            period = (float)periodMs;
            frequency = 1000.0f / period;
            acPresent = true;
            lastTime = now;
        }
    } else {
        acPresent = false;
    }
}

