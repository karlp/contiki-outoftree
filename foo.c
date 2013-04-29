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

#include "contiki.h"
#include "leds.h"

#include <stdio.h> /* For printf() */
#include "blipper.h"
#include "serial-shell.h"
#include "shell-ps.h"
/*---------------------------------------------------------------------------*/
PROCESS(foo_process, "Hello world process");
PROCESS(callback_process, "callback process");
AUTOSTART_PROCESSES(&foo_process, &callback_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(foo_process, ev, data)
{
  PROCESS_BEGIN();

  printf("Hello foo world\n");
  leds_blink();
  serial_shell_init();
  shell_ps_init();
  shell_blink_init();
  shell_powertrace_init();
  static struct blipper_info bl1 = { CLOCK_SECOND * 2, 'a' };
  static struct blipper_info bl2 = { CLOCK_SECOND * 5, 'b' };
  process_start(&blipper_process, (void*)&bl1);
  process_start(&blipper2_process, (void*)&bl2);
  
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/

struct ctimer ct;
static void callback_demo(void *in) {
	printf("In the callback!\n");
	leds_blink();
}

PROCESS_THREAD(callback_process, ev, data)
{
	PROCESS_BEGIN();

	printf("callback about to be set...\n");
	ctimer_set(&ct, 3 * CLOCK_SECOND, callback_demo, (void*) NULL);
	
	PROCESS_END();
}
