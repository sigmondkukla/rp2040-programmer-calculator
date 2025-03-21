#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include <u8g2.h>
#include "peripherals.h"
#include "tca8418.h"

u8g2_t u8g2;

// u8g2 HAL
uint8_t u8x8_byte_pico_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
uint8_t u8x8_gpio_and_delay_pico(u8x8_t *u8x8, uint8_t msg,uint8_t arg_int, void *arg_ptr);

// init
void init_oled();
void init_matrix();

// graphics
void draw_display();

// matrix
void gpio_callback(uint gpio, uint32_t events);
void TCA8418_interrupt_handler(void);

int main()
{
    stdio_init_all();

    init_matrix();
    init_oled();

    draw_display();
}

// u8g2 and graphics
uint8_t u8x8_byte_pico_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
  uint8_t *data;
  switch (msg) {
  case U8X8_MSG_BYTE_SEND:
    data = (uint8_t *)arg_ptr;
    spi_write_blocking(OLED_SPI_PORT, data, arg_int);
    break;
  case U8X8_MSG_BYTE_INIT:
    u8x8_gpio_SetCS(u8x8, u8x8->display_info->chip_disable_level);
    break;
  case U8X8_MSG_BYTE_SET_DC:
    u8x8_gpio_SetDC(u8x8, arg_int);
    break;
  case U8X8_MSG_BYTE_START_TRANSFER:
    u8x8_gpio_SetCS(u8x8, u8x8->display_info->chip_enable_level);
    u8x8->gpio_and_delay_cb(u8x8, U8X8_MSG_DELAY_NANO,u8x8->display_info->post_chip_enable_wait_ns, NULL);
    break;
  case U8X8_MSG_BYTE_END_TRANSFER:
    u8x8->gpio_and_delay_cb(u8x8, U8X8_MSG_DELAY_NANO,
                            u8x8->display_info->pre_chip_disable_wait_ns, NULL);
    u8x8_gpio_SetCS(u8x8, u8x8->display_info->chip_disable_level);
    break;
  default:
    return 0;
  }
  return 1;
}

uint8_t u8x8_gpio_and_delay_pico(u8x8_t *u8x8, uint8_t msg,uint8_t arg_int, void *arg_ptr) {
  switch (msg) {
  case U8X8_MSG_GPIO_AND_DELAY_INIT: 
    spi_init(OLED_SPI_PORT, OLED_SPI_SPEED);
    gpio_set_function(OLED_CS, GPIO_FUNC_SIO);
    gpio_set_function(OLED_SPI_CLK, GPIO_FUNC_SPI);
    gpio_set_function(OLED_SPI_DIN, GPIO_FUNC_SPI);
    gpio_init(OLED_RESET);
    gpio_init(OLED_DC);
    gpio_init(OLED_CS);
    gpio_set_dir(OLED_RESET, GPIO_OUT);
    gpio_set_dir(OLED_DC, GPIO_OUT);
    gpio_set_dir(OLED_CS, GPIO_OUT);
    gpio_put(OLED_RESET, 1);
    gpio_put(OLED_CS, 1);
    gpio_put(OLED_DC, 0);
    break;                  
  case U8X8_MSG_DELAY_NANO: // delay arg_int * 1 nano second
    sleep_us(arg_int); // 1000 times slower, though generally fine in practice given rp2040 has no `sleep_ns()`
    break;
  case U8X8_MSG_DELAY_100NANO: // delay arg_int * 100 nano seconds
    sleep_us(arg_int);
    break;
  case U8X8_MSG_DELAY_10MICRO: // delay arg_int * 10 micro seconds
    sleep_us(arg_int * 10);
    break;
  case U8X8_MSG_DELAY_MILLI: // delay arg_int * 1 milli second
    sleep_ms(arg_int);
    break;
  case U8X8_MSG_GPIO_CS: // CS (chip select) pin: Output level in arg_int
    gpio_put(OLED_CS, arg_int);
    break;
  case U8X8_MSG_GPIO_DC: // DC (data/cmd, A0, register select) pin: Output level
    gpio_put(OLED_DC, arg_int);
    break;
  case U8X8_MSG_GPIO_RESET: // Reset pin: Output level in arg_int
    gpio_put(OLED_RESET, arg_int);  // printf("U8X8_MSG_GPIO_RESET %d\n", arg_int);
    break;
  default:
    u8x8_SetGPIOResult(u8x8, 1); // default return value
    break;
  }
  return 1;
}

void draw_display() {
	char hey[] = "Hello, world!";
	u8g2_ClearBuffer(&u8g2);
    u8g2_ClearDisplay(&u8g2);
	u8g2_SetDrawColor(&u8g2, 1);
    u8g2_SetFont(&u8g2, u8g2_font_t0_11_te);
    u8g2_DrawStr(&u8g2, 10, 10, hey);
    u8g2_UpdateDisplay(&u8g2);
}

// init
void init_oled(){
  u8g2_Setup_ssd1322_nhd_256x64_f(&u8g2, U8G2_R0, u8x8_byte_pico_hw_spi, u8x8_gpio_and_delay_pico); // or instead of nhd, zjy
  u8g2_InitDisplay(&u8g2); // send init sequence to the display, display is in sleep mode after this,
  u8g2_SetPowerSave(&u8g2, 0); 
}

void init_matrix() {
  // Setup TCA8418
  TCA8418_init();
  TCA8418_matrix(8, 10);
  TCA8418_set_interrupt(1);
  TCA8418_set_debounce(1);

  // Setup interrupt pin
  gpio_init(TCA8418_INT); // initialize GPIO pin
  gpio_set_dir(TCA8418_INT, GPIO_IN); // configure as input
  gpio_set_irq_enabled_with_callback(TCA8418_INT, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
}

void gpio_callback(uint gpio, uint32_t events) {
  // determine why the interrupt was triggered
  switch (gpio) {
    case TCA8418_INT:
      TCA8418_interrupt_handler();
      break;
    default:
      printf("Unknown GPIO interrupt on pin %d\n", gpio);
      break;
  }
}

// for now we assume it is only a keypress interrupt
void TCA8418_interrupt_handler(void) {
  uint8_t key = TCA8418_get_key();
  printf("Key pressed: %d\n", key);
}