#ifndef PERIPHERALS_H
#define PERIPHERALS_H

// TCA8418 TCA8418 scanner
#define TCA8418_I2C_PORT i2c1
#define TCA8418_I2C_SPEED 400 * 1000 // 400 kHz
#define TCA8418_I2C_ADDR 0x34
#define TCA8418_I2C_SDA 0
#define TCA8418_I2C_SCL 1
#define TCA8418_INT 2
#define TCA8418_RESET 3

// 4 reserved

// SSD1322 256x64 OLED display
#define OLED_SPI_PORT spi0
#define OLED_SPI_SPEED 3000 * 1000 // 3 MHz, 3.333 MHz max for SSD1322
#define OLED_CS 5
#define OLED_SPI_CLK 6
#define OLED_SPI_DIN 7
#define OLED_DC 8
#define OLED_RESET 9
// 10-13 reserved
#define LED_SRCLK 14
#define LED_SER 15
#define LED_SRCLR 16
#define LED_RCLK 17
#define LED_OE 18

#define CHARGER_STDBY 19
#define CHARGER_CHRG 20
// 21 reserved
#define POWER_EN 22
#define POWER_BTN 23
// 24 reserved
#define BAT_ADC_EN 25
#define BAT_ADC 26
// 27-29 spare GPIO connected to header

#endif