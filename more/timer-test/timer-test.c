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
 *
 * \file
 *         A set of tests for all the timer and clock routines.
 * Requires host side timestamping to help validate.  Partly based on the cc253x
 * timer test routines
 */
#include "contiki.h"
#include "sys/clock.h"
#include "sys/rtimer.h"

#include <stdio.h>

PROCESS(test_etimer, "etimer and ctimer tests");
PROCESS(test_clock_usecs, "clock low level routine tests");
PROCESS(test_clock_wait, "clock low level routine tests2");
PROCESS(foo_process, "test runner process");
AUTOSTART_PROCESSES(&foo_process);


PROCESS_THREAD(test_etimer, ev, data)
{
	static int tick_count = 0;
	static int ticks = 0;
	PROCESS_BEGIN();
	tick_count = *(int *) data;
	printf("TESTSTART{\"expected\":%d}\n", tick_count * 100);

	static struct etimer et; // Define the timer

	etimer_set(&et, 100);
	for (; ticks < tick_count; ticks++) {
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
		printf("etimer tick %d/%d\n", ticks, tick_count);
		etimer_reset(&et);
	}

	printf("TESTEND\n");
	PROCESS_END();
}

PROCESS_THREAD(test_clock_usecs, ev, data)
{
	static int tick_count = 0;
	static int ticks = 0;
	int k;
	PROCESS_BEGIN();
	tick_count = *(int *) data;
	printf("TESTSTART{\"expected\":%d}\n", tick_count * 100);

	for (; ticks < tick_count; ticks++) {
		for (k = 0; k < 100; k++) {
			clock_delay_usec(1000);
		}
		printf("clock_delay_usec: 100 * 1000usecs, %d/%d\n", ticks, tick_count);
	}

	printf("TESTEND\n");
	PROCESS_END();
}

PROCESS_THREAD(test_clock_wait, ev, data)
{
	PROCESS_BEGIN();
	printf("TESTSTART{\"expected\":5000}\n");
	clock_wait(5 * CLOCK_SECOND);
	printf("TESTEND\n");
	PROCESS_END();
}

void
test_etimer_init(int i)
{
	process_start(&test_etimer, (void *) &i);
}

void
test_clock_usecs_init(int i)
{
	process_start(&test_clock_usecs, (void *) &i);
}


/*---------------------------------------------------------------------------*/
PROCESS_THREAD(foo_process, ev, data)
{
	PROCESS_BEGIN();

	printf("Hello foo world\n");
	test_clock_usecs_init(10);
	PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_EXITED);
	test_etimer_init(10);
	PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_EXITED);
	process_start(&test_clock_wait, NULL);
	PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_EXITED);

	PROCESS_END();
}

