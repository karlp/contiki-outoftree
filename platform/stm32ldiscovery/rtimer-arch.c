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
#include <libopencm3/stm32/rtc.h>

static volatile rtimer_clock_t next_trigger;
static uint32_t now_msb;
static uint32_t next_alarm;

#define RTC_RELOAD_IDLE 0xffff

void rtc_wkup_isr(void)
{
	ENERGEST_ON(ENERGEST_TYPE_IRQ);

	// If this was a periodic isr, just update the time.
	// otherwise, we need to run tasks...


	// but we also need to wakeup at set times

	/* clear flag, not write protected */
	RTC_ISR &= ~(RTC_ISR_WUTF);
	exti_reset_request(EXTI20);
	/*
	 * Make sure we're really awake! We need to do this
	 * before calling the next rtimer_task, since the task may need the RF.
	 */
	//lpm_exit();

	rtimer_run_next();

	ENERGEST_OFF(ENERGEST_TYPE_IRQ);

}

/**
 * \brief Turn on the RTC periph and prepare as much as possible
 *
 *        The Sleep Timer starts ticking automatically as soon as the device
 *        turns on. We don't need to turn on interrupts before the first call
 *        to rtimer_arch_schedule()
 */
void
rtimer_arch_init(void)
{
	/* turn on power block to enable unlocking */
	rcc_peripheral_enable_clock(&RCC_APB1ENR, RCC_APB1ENR_PWREN);
	pwr_disable_backup_domain_write_protect();

	/* reset rtc */
	RCC_CSR |= RCC_CSR_RTCRST;
	RCC_CSR &= ~RCC_CSR_RTCRST;

	/* LSE vs LSI (hint: you want LSE)*/
#if (RTIMER_EXTERNAL_CRYSTAL == 1)
	rcc_osc_on(LSE);
	rcc_wait_for_osc_ready(LSE);
	rcc_rtc_select_clock(RCC_CSR_RTCSEL_LSE);
#else
	rcc_osc_on(LSI);
	rcc_wait_for_osc_ready(LSI);
	rcc_rtc_select_clock(RCC_CSR_RTCSEL_LSI);
#endif

	/* ?! Stdperiph examples don't turn this on until _afterwards_ which
	 * simply doesn't work.  It must be on at least to be able to configure it */
	RCC_CSR |= RCC_CSR_RTCEN;

	rtc_unlock();
	/* enter init mode */
	RTC_ISR |= RTC_ISR_INIT;
	while ((RTC_ISR & RTC_ISR_INITF) == 0)
		;

	/* We don't need these for wakeup rtimers, so don't set them.
	  (These are the defaults for 1hz out)
	u32 sync = 255;
	u32 async = 127;
	rtc_set_prescaler(sync, async);
	 */

	/* exit init mode */
	RTC_ISR &= ~(RTC_ISR_INIT);

	/* and write protect again */
	rtc_lock();

	/* and finally enable the clock */
	RCC_CSR |= RCC_CSR_RTCEN;

	/* And wait for synchro.. */
	rtc_wait_for_synchro();
}

static int setup_rtc_wakeup(int ticks)
{
	rtc_unlock();

	/* ensure wakeup timer is off */
	RTC_CR &= ~RTC_CR_WUTE;

	/* Wait until we can write */
	while ((RTC_ISR & RTC_ISR_WUTWF) == 0)
		;

	RTC_WUTR = ticks - 1;

	/* 
	 * Use the fastest clock we have, to get the most resolution
	 * contiki uses this for "short" times, not for hours and days.
	 */
	RTC_CR &= ~(RTC_CR_WUCLKSEL_MASK << RTC_CR_WUCLKSEL_SHIFT);
	RTC_CR |= (RTC_CR_WUCLKSEL_RTC_DIV2 << RTC_CR_WUCLKSEL_SHIFT);

	/* Restart WakeUp unit */
	RTC_CR |= RTC_CR_WUTE;

	/* Enable wakeup when we're finished */
	RTC_CR |= RTC_CR_WUTIE;

	/* done with rtc registers, lock them again */
	rtc_lock();
	nvic_enable_irq(NVIC_RTC_WKUP_IRQ);

	/* EXTI line 20 is the wakeup interrupt */
	exti_set_trigger(EXTI20, EXTI_TRIGGER_RISING);
	exti_enable_request(EXTI20);
	return 0;
}

/**
 * \brief Returns the current real-time clock time
 * \return The current rtimer time in ticks.
 * This is whatever we have stored from before last interrupts, plus where the
 * rtc timer is currently up to. (remembering that it's a down counter)
 */
rtimer_clock_t
rtimer_arch_now()
{
	uint32_t extra = RTC_RELOAD_IDLE - RTC_WUTR;
	return now_msb + extra;
}

void
rtimer_arch_schedule(rtimer_clock_t t)
{
}