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
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

/**
 * \file
 *         A test bed for rtimer implementations.
 *         An etimer triggers ever 3 seconds, and prints out the rtimer_now value
 */

#include <stdio.h> 
#include "contiki.h"
#include <sys/clock.h>
#include <sys/etimer.h>
#include <sys/rtimer.h>

/*---------------------------------------------------------------------------*/
PROCESS(rtimer_simple_process, "periodic rtimer_simple process");
PROCESS(foo_process, "bah");
AUTOSTART_PROCESSES(&foo_process);

PROCESS_THREAD(foo_process, ev, data)
{
	PROCESS_BEGIN();

	printf("Hello foo world\n");
	process_start(&rtimer_simple_process, NULL);

	PROCESS_END();
}

PROCESS_THREAD(rtimer_simple_process, ev, data)
{
	static struct etimer et;
	static int ticks;
	PROCESS_BEGIN();
	printf("Hello rtimer_simple\n");

	/* Delay 1 second */
	etimer_set(&et, CLOCK_SECOND / 4);

	while (1) {
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
		/* Reset the etimer to trig again in 1 second */
		rtimer_clock_t now = RTIMER_NOW();
		printf("tick: %d clock time: %lu, rtimer: %lu (%#x)\n",
			ticks, clock_seconds(), now, now);
		ticks++;
		etimer_reset(&et);
		/* ... */
	}
	PROCESS_END();
}