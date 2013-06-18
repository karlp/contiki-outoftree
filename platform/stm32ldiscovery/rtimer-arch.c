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
 * \addtogroup stm32-ocm3-rtimer
 * @{
 *
 * \file
 * Implementation of the arch-specific rtimer functions for the stm32 L1
 * platform.  
 */
#include "contiki.h"
#include "sys/energest.h"
#include "sys/rtimer.h"

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/pwr.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>

static volatile rtimer_clock_t next_trigger;
static volatile rtimer_clock_t latest_hard_count;
static volatile int rtimer_late_count = 0;

#define TIMER_RTIMER TIM11
#define TIMER_RELOAD_IDLE 0xffff

void tim11_isr()
{
	ENERGEST_ON(ENERGEST_TYPE_IRQ);
	if (timer_get_flag(TIMER_RTIMER, TIM_SR_UIF)) {
		timer_clear_flag(TIMER_RTIMER, TIM_SR_UIF);
		latest_hard_count += TIMER_RELOAD_IDLE + 1;
		int epoch_next = next_trigger >> 16;
		int epoch_now = latest_hard_count >> 16;
		if (epoch_next == epoch_now) {
			// we've reached the right "epoch", turn on the 
			// capture compare for the remainder!
			timer_set_oc_value(TIMER_RTIMER, TIM_OC1, next_trigger & 0xffff);
			// TODO - put this in the init code?
			timer_set_oc_mode(TIMER_RTIMER, TIM_OC1, TIM_OCM_ACTIVE);
			timer_enable_irq(TIMER_RTIMER, TIM_DIER_CC1IE);
		} 
//		else if (epoch_next < epoch_now) {
//			// SHIT!
//			rtimer_late_count++;
//			rtimer_run_next();
//		}
	}
	
	if (timer_get_flag(TIMER_RTIMER, TIM_SR_CC1IF)) {
		timer_clear_flag(TIMER_RTIMER, TIM_SR_CC1IF);
		timer_disable_irq(TIMER_RTIMER, TIM_DIER_CC1IE);
		rtimer_run_next();
	}
	ENERGEST_OFF(ENERGEST_TYPE_IRQ);
}

/**
 * \brief Setup as much as we can of the hard timing infrastructure
 *
 */
void
rtimer_arch_init(void)
{
	latest_hard_count = 0;
	rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_TIM11EN);
	nvic_enable_irq(NVIC_TIM11_IRQ);

	// joy of joys, conflict namespace with contiki:
	// timer_reset(TIMER_RTIMER);
	rcc_peripheral_reset(&RCC_APB2RSTR, RCC_APB2RSTR_TIM11RST);
	rcc_peripheral_clear_reset(&RCC_APB2RSTR, RCC_APB2RSTR_TIM11RST);

	/* TODO - is it even worth allowing running this off LSI? (It needs lots
	 * of juggling the inputs, and it's probably not accurate enough anyway)
	 */
#if (RTIMER_EXTERNAL_CRYSTAL == 0)
#error Running the rtimer off the LSI is not yet supported
#else
	rcc_peripheral_enable_clock(&RCC_APB1ENR, RCC_APB1ENR_PWREN);
	pwr_disable_backup_domain_write_protect();
	rcc_osc_on(LSE);
	rcc_wait_for_osc_ready(LSE);
#endif

	// Enable external clock mode 2 (ETR), no filtering, no clock div.
	TIM_SMCR(TIMER_RTIMER) = TIM_SMCR_ECE | TIM_SMCR_ETPS_OFF | TIM_SMCR_ETF_OFF;
	/*
	 *  From AN4013
	 * Update_event = ETR_CLK/((ETR_PSC)*(PSC + 1)*(ARR + 1)*(RCR + 1))
	 * 32768/(1*(0+1)*(0xffff+1)*(1)
	 * = 0.5Hz when in idle mode.
	 */
	// 
	timer_set_prescaler(TIMER_RTIMER, 0); // PSC
	timer_set_period(TIMER_RTIMER, TIMER_RELOAD_IDLE);
	timer_enable_counter(TIMER_RTIMER);
	timer_enable_irq(TIMER_RTIMER, TIM_DIER_UIE);
}

/**
 * \brief Returns the current real-time clock time
 * \return The current rtimer time in ticks.
 * This is whatever we have stored from before last interrupts, plus where the
 * timer is currently up to.
 */
rtimer_clock_t
rtimer_arch_now()
{
	uint32_t extra = timer_get_counter(TIMER_RTIMER);
	return latest_hard_count + extra;
}

void
rtimer_arch_schedule(rtimer_clock_t t)
{
	next_trigger = t;
}