/*
 * Copyright (c) 2013, Karl Palsson <karlp@tweak.net.au>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include "dev/leds.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>


#define LED_DISCO_PORT GPIOB
#define LED_DISCO_PIN_GREEN GPIO7
#define LED_DISCO_PIN_BLUE GPIO6

void leds_arch_init(void)
{
	rcc_peripheral_enable_clock(&RCC_AHBENR, RCC_AHBENR_GPIOBEN);
	gpio_mode_setup(LED_DISCO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,
		LED_DISCO_PIN_BLUE | LED_DISCO_PIN_GREEN);
}

unsigned char leds_arch_get(void)
{
	unsigned char ret = 0;
	u16 reg = gpio_get(LED_DISCO_PORT, LED_DISCO_PIN_BLUE | LED_DISCO_PIN_GREEN);
	if (reg & LED_DISCO_PIN_BLUE) {
		ret |= LEDS_BLUE;
	}
	if (reg & LED_DISCO_PIN_GREEN) {
		ret |= LEDS_GREEN;
	}
	return ret;
}

void leds_arch_set(unsigned char leds)
{
	u32 reg = GPIO_ODR(LED_DISCO_PORT);
	reg &= ~(LED_DISCO_PIN_GREEN | LED_DISCO_PIN_BLUE);
	if (leds & LEDS_GREEN) {
		reg |= LED_DISCO_PIN_GREEN;
	}
	if (leds & LEDS_BLUE) {
		reg |= LED_DISCO_PIN_BLUE;
	}
	GPIO_ODR(LED_DISCO_PORT) = reg;
}

