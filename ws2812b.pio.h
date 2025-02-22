// -------------------------------------------------- //
// This file is autogenerated by pioasm; do not edit! //
// -------------------------------------------------- //

#pragma once

#if !PICO_NO_HARDWARE
#include "hardware/pio.h"
#endif

// ------- //
// ws2812b //
// ------- //

#define ws2812b_wrap_target 0
#define ws2812b_wrap 6
//#define ws2812b_pio_version 0

static const uint16_t ws2812b_program_instructions[] = {
            //     .wrap_target
    0x6021, //  0: out    x, 1                       
    0x0024, //  1: jmp    !x, 4                      
    0xe401, //  2: set    pins, 1                [4] 
    0x0006, //  3: jmp    6                          
    0xe201, //  4: set    pins, 1                [2] 
    0xe200, //  5: set    pins, 0                [2] 
    0xe100, //  6: set    pins, 0                [1] 
            //     .wrap
};

#if !PICO_NO_HARDWARE
static const struct pio_program ws2812b_program = {
    .instructions = ws2812b_program_instructions,
    .length = 7,
    .origin = -1,
//    .pio_version = ws2812b_pio_version,
#if PICO_PIO_VERSION > 0
    .used_gpio_ranges = 0x0
#endif
};

static inline pio_sm_config ws2812b_program_get_default_config(uint offset) {
    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_wrap(&c, offset + ws2812b_wrap_target, offset + ws2812b_wrap);
    return c;
}

static inline void ws2812b_program_init(PIO pio, uint sm, uint offset, uint pin)
{
    pio_sm_config c = ws2812b_program_get_default_config(offset);
    // Set pin to be part of set output group, i.e. set by set instruction
    sm_config_set_set_pins(&c, pin, 1);
    // Attach pio to the GPIO
    pio_gpio_init(pio, pin);
    // Set pin direction to output at the PIO
    pio_sm_set_consecutive_pindirs(pio, sm, pin, 1, true);
    // Set pio clock to 8MHz, giving 10 cycles per LED binary digit
    float div = clock_get_hz(clk_sys) / 8000000.0;
    sm_config_set_clkdiv(&c, div);
    // Give all the FIFO space to TX (not using RX)
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX);
    // Shift to the left, use autopull, next pull threshold 24 bits
    sm_config_set_out_shift(&c, false, true, 24);
    // Set sticky-- continue to drive value from last set/out.  Other stuff off.
    sm_config_set_out_special(&c, true, false, false);
    // Load configuration, and jump to the start of the program
    pio_sm_init(pio, sm, offset, &c);
    // enable this pio state machine
    pio_sm_set_enabled(pio, sm, true);
}

#endif

