/*
 * Copyright (c) 2013, Karl Palsson <karlp@tweak.net.au>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/**
 * \file   mrf24j40-arch.c
 * 
 * \brief  MRF24J40 arch driver for stm32-ocm3 platform
 * Mostly ripped from https://github.com/karlp/simrf/blob/master/examples/stm32_opencm3/platform_l1.c
 * \author Karl Palsson <karlp@tweak.net.au>
 */

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/spi.h>

#include "dev/mrf24j40.h"

#define MRF_SPI SPI2
#define MRF_SPI_PORT GPIOB
#define MRF_SELECT_PORT GPIOB
#define MRF_SELECT_PIN GPIO12
#define MRF_RESET_PORT GPIOA
#define MRF_RESET_PIN GPIO5
#define MRF_INTERRUPT_PORT GPIOB
#define MRF_INTERRUPT_PIN GPIO2
#define MRF_INTERRUPT_NVIC NVIC_EXTI2_IRQ
#define MRF_INTERRUPT_EXTI EXTI2


void mrf24j40_arch_wake_pin(int val)
{
	// FIXME - TODO
}

/**
 * @param val logical value for asserting reset.
 * FIXME - make this proper doxygen style!
 */
void mrf24j40_arch_hard_reset(int val)
{
	// active low
        if (val) {
                gpio_clear(MRF_RESET_PORT, MRF_RESET_PIN);
        } else {
                gpio_set(MRF_RESET_PORT, MRF_RESET_PIN);
        }
}

int mrf24j40_arch_init(void)
{
	/* clocks */
	rcc_peripheral_enable_clock(&RCC_APB1ENR, RCC_APB1ENR_SPI2EN);
        rcc_peripheral_enable_clock(&RCC_AHBENR, RCC_AHBENR_GPIOAEN);
        rcc_peripheral_enable_clock(&RCC_AHBENR, RCC_AHBENR_GPIOBEN);
	
	/* gpios */
        gpio_mode_setup(MRF_RESET_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, MRF_RESET_PIN);
        gpio_mode_setup(MRF_INTERRUPT_PORT, GPIO_MODE_INPUT, GPIO_PUPD_NONE, MRF_INTERRUPT_PIN);

	/* spi */
        gpio_mode_setup(MRF_SPI_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO13 | GPIO14 | GPIO15);
        gpio_set_af(MRF_SPI_PORT, GPIO_AF5, GPIO13 | GPIO14 | GPIO15);
        gpio_set_output_options(MRF_SPI_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO13 | GPIO14 | GPIO15);
        gpio_mode_setup(MRF_SELECT_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, MRF_SELECT_PIN);

        /* Setup SPI parameters. */
        spi_init_master(MRF_SPI, SPI_CR1_BAUDRATE_FPCLK_DIV_4, SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
                SPI_CR1_CPHA_CLK_TRANSITION_1, SPI_CR1_DFF_8BIT, SPI_CR1_MSBFIRST);
        /* Ignore the stupid NSS pin. */
        spi_enable_software_slave_management(MRF_SPI);
        spi_set_nss_high(MRF_SPI);
        /* Finally enable the SPI. */
        spi_enable(MRF_SPI);

	
	
	return 0;
}

int mrf24j40_arch_is_irq_enabled(void)
{
	return nvic_get_irq_enabled(MRF_INTERRUPT_NVIC);
}

void mrf24j40_arch_irq_enable(void)
{
        // Enable EXTI interrupt.
        nvic_enable_irq(MRF_INTERRUPT_NVIC);
        /* Configure the EXTI subsystem. */
        exti_select_source(MRF_INTERRUPT_EXTI, MRF_INTERRUPT_PORT);
        exti_set_trigger(MRF_INTERRUPT_EXTI, EXTI_TRIGGER_FALLING);
        exti_enable_request(MRF_INTERRUPT_EXTI);	
}

void mrf24j40_arch_irq_disable(void)
{
	exti_disable_request(MRF_INTERRUPT_EXTI);
        nvic_disable_irq(MRF_INTERRUPT_NVIC);
}

void mrf24j40_arch_select(void)
{
	gpio_clear(MRF_SELECT_PORT, MRF_SELECT_PIN);
}

void mrf24j40_arch_deselect(void)
{
	gpio_set(MRF_SELECT_PORT, MRF_SELECT_PIN);
}

void exti2_isr(void)
{
        exti_reset_request(MRF_INTERRUPT_EXTI);
	mrf24j40_irq_handler();
}

void mrf24j40_arch_spi_write(uint8_t *data, int length) {
	int i;
	for (i = 0; i < length; i++) {
		spi_xfer(MRF_SPI, data[i]);
	}
}

void mrf24j40_arch_spi_read(uint8_t *data, int length) {
	int i;
	for (i = 0; i < length; i++) {
		data[i] = spi_xfer(MRF_SPI, 0);
	}
}