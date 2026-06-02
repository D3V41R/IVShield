#include "main.h"
#include "stdio.h"
#include "cmsis_os.h"
#include "stdint.h"
#include "stm32f4xx_hal.h"

extern UART_HandleTypeDef huart2;
extern ADC_HandleTypeDef hadc1;
extern I2C_HandleTypeDef hi2c1;

#define LCD_ADDR (0x27 << 1)
#define LCD_COLS 16
#define LCD_ROWS 2

void Lcd_SendCmd(uint8_t cmd);
void Lcd_SendData(uint8_t data);
void Lcd_SendString(char *str);
void Lcd_SetCursor(uint8_t row, uint8_t col);
void Lcd_Init(void);

void Lcd_Write(uint8_t data, uint8_t rs){
    uint8_t highNibble = (data & 0xF0) | 0x08 | (rs ? 0x01 : 0x00);
    uint8_t lowNibble  = ((data << 4) & 0xF0) | 0x08 | (rs ? 0x01 : 0x00);

    uint8_t buf[4];
    buf[0] = highNibble | 0x04;
    buf[1] = highNibble;
    buf[2] = lowNibble | 0x04;
    buf[3] = lowNibble;

    HAL_I2C_Master_Transmit(&hi2c1, LCD_ADDR, buf, 4, HAL_MAX_DELAY);
}

void Lcd_SendCmd(uint8_t cmd){
    Lcd_Write(cmd, 0);
}

void Lcd_SendData(uint8_t data){
    Lcd_Write(data, 1);
}

void Lcd_SendString(char *str){
    while(*str) Lcd_SendData(*str++);
}

void Lcd_SetCursor(uint8_t row, uint8_t col){
    uint8_t pos = (row == 0) ? col : (0x40 + col);
    Lcd_SendCmd(0x80 | pos);
}

void Lcd_Init(void){
    HAL_Delay(50);
    Lcd_SendCmd(0x33);
    Lcd_SendCmd(0x32);
    Lcd_SendCmd(0x28);  // 4-bit, 2 lines
    Lcd_SendCmd(0x0C);  // display on, cursor off
    Lcd_SendCmd(0x06);  // entry mode
    Lcd_SendCmd(0x01);  // clear display
    HAL_Delay(5);
}
