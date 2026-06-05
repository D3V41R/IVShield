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

    if(HAL_GPIO_ReadPin(Cross_Detector_GPIO_Port, Cross_Detector_Pin) == GPIO_PIN_SET){
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
