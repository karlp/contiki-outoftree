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

PROCESS(foo_process, "bah");
PROCESS(eetimer_process, "periodic etimer_simple process");
PROCESS(rrtimer_process, "periodic rtimer_simple process");
PROCESS(timer_test_process, "test code for timing comparisons");
AUTOSTART_PROCESSES(&foo_process);

static struct rtimer rt;
static int rticks;

PROCESS_THREAD(foo_process, ev, data)
{
	PROCESS_BEGIN();

	printf("Hello foo world\n");

	process_start(&eetimer_process, NULL);
	process_start(&rrtimer_process, NULL);
	//process_start(&timer_test_process, NULL);

	PROCESS_END();
}

/**
 * 
 * @param process_pt
 * @param ev
 * @param data
 * @return 
 */
PROCESS_THREAD(eetimer_process, ev, data)
{
	static struct etimer et;
	static int ticks;
	PROCESS_BEGIN();
	printf("EEE: Hello etimer\n");
	etimer_set(&et, CLOCK_SECOND / 4);
	while (1) {
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
		/* Reset the etimer to trig again in 1 second */
		rtimer_clock_t now = RTIMER_NOW();
		float nowf = now / (RTIMER_SECOND * 1.0);
		printf("EEE: tick: %d clock time: %lu, rtimer: %lu (%#lx), nowf: %f\n",
			ticks, clock_seconds(), now, now, nowf);
		ticks++;
		etimer_reset(&et);
	}
	PROCESS_END();
}

void
rt_callback(struct rtimer *t, void *ptr)
{
	rtimer_clock_t now = RTIMER_NOW();
	float nowf = now / (RTIMER_SECOND * 1.0);
	printf("RRRR: %c tick %d clock: %lu, rtimer: %lu (%#lx), nowf: %f\n",
		*(char *)ptr, rticks, clock_seconds(), now, now, nowf);
	rticks++;
	rtimer_set(t, RTIMER_TIME(t) + (RTIMER_SECOND / 4), 1,
		rt_callback, ptr);

}

PROCESS_THREAD(rrtimer_process, ev, data)
{
	static char demo_data = 'k';
	PROCESS_BEGIN();
	printf("RRRR_a: Hello rtimer\n");
	rtimer_set(&rt, RTIMER_NOW() + (RTIMER_SECOND >> 1), 1,
		rt_callback, &demo_data);

	PROCESS_END();
}

PROCESS_THREAD(timer_test_process, ev, data)
{

	static struct etimer et;
	PROCESS_BEGIN();

	etimer_set(&et, 2 * CLOCK_SECOND);

	PROCESS_YIELD();

	printf("-----------------------------------------\n");
	printf("clock_delay_usec test, (10,000 x i) usec:\n");
	printf("N.B. clock_delay_usec is more accurate than rtimers\n");
	int i = 1;
	while (i < 7) {
		rtimer_clock_t start_count = RTIMER_NOW();
		//clock_delay_usec(10000 * i);
		clock_delay_usec(10000);
		rtimer_clock_t end_count = RTIMER_NOW();
		rtimer_clock_t diff = end_count - start_count;
		printf("Requested: %u usec, Real: %lu rtimer ticks = ~%lu us\n",
			10000 * i, diff, diff * 1000000 / RTIMER_SECOND);
		i++;
	}

	printf("Done!\n");

	PROCESS_END();
}
