#include "bit_leds.h"
#include "peripherals.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"

/// @brief Init the bit LEDs, setting up the GPIOs for shift register control lines and clearing the LEDs.
void bit_leds_init()
{
    // init all shift register GPIOs
    gpio_init(LED_SRCLK);
    gpio_init(LED_SER);
    gpio_init(LED_SRCLR);
    gpio_init(LED_RCLK);
    gpio_init(LED_OE);

    // Set shift register control lines to output
    gpio_set_dir(LED_SRCLK, true);
    gpio_set_dir(LED_SER, true);
    gpio_set_dir(LED_SRCLR, true);
    gpio_set_dir(LED_RCLK, true);
    gpio_set_dir(LED_OE, true);

    // Clear and set default values
    bit_leds_clear();
    bit_leds_latch();
    bit_leds_enable(true);

    // Init serial clock and data low too
    gpio_put(LED_SRCLK, false);
    gpio_put(LED_SER, false);
}

void bit_leds_enable(bool enable)
{
    gpio_put(LED_OE, !enable);
}

/// @brief Clear all LEDs, pulsing SRCLR and RCLK.
void bit_leds_clear()
{
    gpio_put(LED_SRCLR, true);
    sleep_us(1);
    gpio_put(LED_SRCLR, false);
    sleep_us(1);
    gpio_put(LED_SRCLR, true);
    sleep_us(1);
}

/// @brief Latch shift register to storage register for output
void bit_leds_latch()
{
    gpio_put(LED_RCLK, false);
    sleep_us(1);
    gpio_put(LED_RCLK, true);
    sleep_us(1);
    gpio_put(LED_RCLK, false);
    sleep_us(1);
}

/// @brief Lights the LEDs corresponding to the provided 32-bit value
/// @param value The number to display in binary on the LEDs
void bit_leds_set(uint32_t value)
{
    // The LEDs are wired kind of funny. In hindsight not very smart.
    // Each shift register QA is the MSB, and QG is the LSB. Then QH wraps
    // to SER on the next shift register for the next 8 bits. The problem is
    // that the least significant byte has the serial data as opposed to the
    // most significant byte. Oh well.
    // Either way we first must send out byte 3, then 2, then 1, then 0. But
    // don't reverse any of the individual bits.

    for (int i = 3; i >= 0; i--)
    {                                             // for each byte, most significant to least significant
        uint8_t byte = (value >> (i * 8)) & 0xFF; // extract current byte

        for (int j = 7; j >= 0; j--)
        {                               // send each bit, MSB first. This may need to be changed later.
            bool bit = (byte >> j) & 1; // extract the current bit

            gpio_put(LED_SER, bit); // set SER to current bit
            sleep_us(1);            // set up time

            // Pulse SRCLK
            gpio_put(LED_SRCLK, true);
            sleep_us(1);
            gpio_put(LED_SRCLK, false);
            sleep_us(1);
        }
    }

    bit_leds_latch(); // latch shifted data for output
}
