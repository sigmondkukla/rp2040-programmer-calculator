#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "tca8418.h"
#include "peripherals.h"

void TCA8418_init(void) {
  // Init I2C
  i2c_init(TCA8418_I2C_PORT, TCA8418_I2C_SPEED); // 400kHz

  gpio_set_function(TCA8418_I2C_SDA, GPIO_FUNC_I2C);
  gpio_set_function(TCA8418_I2C_SCL, GPIO_FUNC_I2C);
  gpio_pull_up(TCA8418_I2C_SDA);
  gpio_pull_up(TCA8418_I2C_SCL);

  // Set defaults
  // Set GPIO direction to input
  TCA8418_write_register(TCA8418_REG_GPIO_DIR_1, 0x00);
  TCA8418_write_register(TCA8418_REG_GPIO_DIR_2, 0x00);
  TCA8418_write_register(TCA8418_REG_GPIO_DIR_3, 0x00);

  // Add all pins to key events
  TCA8418_write_register(TCA8418_REG_GPI_EM_1, 0xFF);
  TCA8418_write_register(TCA8418_REG_GPI_EM_2, 0xFF);
  TCA8418_write_register(TCA8418_REG_GPI_EM_3, 0xFF);

  // Set all pins to falling edge interrupt
  TCA8418_write_register(TCA8418_REG_GPIO_INT_LVL_1, 0x00);
  TCA8418_write_register(TCA8418_REG_GPIO_INT_LVL_2, 0x00);
  TCA8418_write_register(TCA8418_REG_GPIO_INT_LVL_3, 0x00);

  // Add all pins to interrupts
  TCA8418_write_register(TCA8418_REG_GPIO_INT_EN_1, 0xFF);
  TCA8418_write_register(TCA8418_REG_GPIO_INT_EN_2, 0xFF);
  TCA8418_write_register(TCA8418_REG_GPIO_INT_EN_3, 0xFF);
}

void TCA8418_matrix(uint8_t rows, uint8_t columns){
  if ((rows > 8) || (columns > 10)) return; // Max 8 rows and 10 columns
  if ((rows == 0) || (columns == 0)) return; // No rows or columns

  uint8_t mask = 0x00;
  for (int r = 0; r < rows; r++) {
    mask <<= 1;
    mask |= 1;
  }
  TCA8418_write_register(TCA8418_REG_KP_GPIO_1, mask);

  mask = 0x00;
  for (int c = 0; c < columns && c < 8; c++) {
    mask <<= 1;
    mask |= 1;
  }
  TCA8418_write_register(TCA8418_REG_KP_GPIO_2, mask);

  if (columns > 8) {
    if (columns == 9) {
      mask = 0x01;
    }  else {
      mask = 0x03;
      TCA8418_write_register(TCA8418_REG_KP_GPIO_3, mask);
    }
  }
}

uint8_t TCA8418_read_register(uint8_t reg) {
  uint8_t value;
  i2c_write_blocking(TCA8418_I2C_PORT, TCA8418_I2C_ADDR, &reg, 1, true);
  i2c_read_blocking(TCA8418_I2C_PORT, TCA8418_I2C_ADDR, &value, 1, false);
  return value;
}

void TCA8418_write_register(uint8_t reg, uint8_t value) {
  uint8_t data[2] = {reg, value};
  i2c_write_blocking(TCA8418_I2C_PORT, TCA8418_I2C_ADDR, data, sizeof(data), false);
}

// Get number of key events in buffer
uint8_t TCA8418_available(void){
  return TCA8418_read_register(TCA8418_REG_LCK_EC_KLEC_0) & 0x0F; // event count is in lower 4 bits
}

// Get key event from buffer
// Returns 0 if no event
// bits 6:0 are the key that was pressed
// bit 7 is the event type (0 = key press, 1 = key release)
uint8_t TCA8418_get_event(void){
  return TCA8418_read_register(TCA8418_REG_KEY_EVENT_A);
}

// Flush key events, returning number of events flushed
uint8_t TCA8418_flush(void){
  uint8_t count = 0;
  while(TCA8418_get_event() != 0) count++;
  return count;
}

// enable/disable interrupt on all keys
// 0 = disable, 1 = enable
void TCA8418_set_interrupt(uint8_t enable) {
  uint8_t cfg = TCA8418_read_register(TCA8418_REG_CFG);
  if (enable) {
    cfg |= TCA8418_REG_CFG_KE_IEN; // enable key events interrupt
  } else {
    cfg &= ~TCA8418_REG_CFG_KE_IEN; // disable key events interrupt
  }
  TCA8418_write_register(TCA8418_REG_CFG, cfg);
}

// enable/disable matrix overflow interrupt
// 0 = disable, 1 = enable
void TCA8418_set_matrix_overflow(uint8_t enable) {
  uint8_t cfg = TCA8418_read_register(TCA8418_REG_CFG);
  if (enable) {
    cfg |= TCA8418_REG_CFG_OVR_FLOW_IEN; // enable overflow interrupt
    cfg |= TCA8418_REG_CFG_OVR_FLOW_M;   // enable overflow mode
  } else {
    cfg &= ~TCA8418_REG_CFG_OVR_FLOW_IEN; // disable overflow interrupt
    cfg &= ~TCA8418_REG_CFG_OVR_FLOW_M;   // disable overflow mode
  }
  TCA8418_write_register(TCA8418_REG_CFG, cfg);
}

// enable/disable debounce on all keys
// 0 = disable, 1 = enable
void TCA8418_set_debounce(uint8_t enable) {
  uint8_t value = enable ? 0xFF : 0x00;
  TCA8418_write_register(TCA8418_REG_DEBOUNCE_DIS_1, value);
  TCA8418_write_register(TCA8418_REG_DEBOUNCE_DIS_2, value);
  TCA8418_write_register(TCA8418_REG_DEBOUNCE_DIS_3, value);
}