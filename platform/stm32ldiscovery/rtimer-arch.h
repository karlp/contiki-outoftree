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
 * \addtogroup stm32-ocm3
 * @{
 *
 * \defgroup stm32-ocm3-rtimer stm32-ocm3 rtimer
 *
 * Implementation of the rtimer module for STM32L1 parts.
 * 
 * Should work on most stm32 platforms with the rtcv2 peripheral.  The general
 * plan is to run the RTC Wake Up timer on LSE or LSI div 2, which is as fast
 * as we can go.  That unfortunately gives us a maximum resolution of ~61uSec.
 * Not ideal, but "good enough"
 * 
 * Unfortunately, the wakeup timer is only 16bit, 
 * 
 *
 * 
 * * @{
 */
/**
 * \file
 * Header file for the stm32-ocm3 rtimer driver
 */
#ifndef RTIMER_ARCH_H_
#define RTIMER_ARCH_H_

#include "contiki.h"

#ifdef RTIMER_CONF_EXTERNAL_CRYSTAL
#define RTIMER_EXTERNAL_CRYSTAL RTIMER_CONF_EXTERNAL_CRYSTAL
#else
#define RTIMER_EXTERNAL_CRYSTAL 1
#endif

#define RTIMER_ARCH_SECOND 32768

/** \sa RTIMER_NOW() */
rtimer_clock_t rtimer_arch_now(void);

/**
 * \brief Get the time of the next scheduled rtimer trigger
 * \return The time next rtimer ISR is scheduled for
 */
rtimer_clock_t rtimer_arch_next_trigger(void);

#endif /* RTIMER_ARCH_H_ */

/**
 * @}
 * @}
 */
