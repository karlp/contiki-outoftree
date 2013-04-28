/*
 * Copyright (c) 2006, Swedish Institute of Computer Science.
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
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
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
 * This file is part of the Contiki operating system.
 *
 */

/**
 * \file
 *         A very simple Contiki application showing how Contiki programs look
 * \author
 *         Adam Dunkels <adam@sics.se>
 */

#include <stdio.h> 
#include "contiki.h"
#include <sys/clock.h>
#include <sys/etimer.h>

#include "dev/leds.h"

#include "blipper.h"
/*---------------------------------------------------------------------------*/
PROCESS(blipper2_process, "periodic blipper2 process");

// Not allowed to be in my own apps, must start them by hand :(
//AUTOSTART_PROCESSES(&blipper_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(blipper2_process, ev, data)
{

	struct blipper_info *blinfo;
	clock_time_t *period;
	static char id;
	PROCESS_BEGIN();
	blinfo = data;
	period = &blinfo->period;
	id = blinfo->id;

	PROCESS_EXITHANDLER(goto exit);

  	printf("Hello blipper2 id: %c!\n", id);

	
	static int ticks = 0;
	static struct etimer et; // Define the timer

	etimer_set(&et, *period);
	printf("Started timer\n");

	while(1) {
		//PROCESS_WAIT_EVENT();
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
		if(etimer_expired(&et)) { 
			printf("%c hit timer expiry tick: %d at clock time: %lu\n", id, ticks, clock_seconds());
			if (ticks % 2 == 0) {
				leds_toggle(LEDS_BLUE);
			} else {
				leds_toggle(LEDS_GREEN);
			}
			etimer_reset(&et);
			ticks++;
		}
		printf("%c Should only print after our event....\n", id);
	}
	
  
exit:
	printf("Exiting....\n");
	PROCESS_END();
}

