#include <stdint.h>
#include <stdio.h>
#include <debug-uart.h>
#include <sys/autostart.h>
#include <sys/clock.h>
#include <sys/ctimer.h>
#include <sys/etimer.h>
#include <sys/process.h>
#include <sys/procinit.h>
#include <dev/serial-line.h>
#include <dev/leds.h>

#include "contiki-conf.h"
#include <libopencm3/stm32/rcc.h>


static void
configure_mcu_clocks(void)
{
	rcc_clock_setup_pll(&clock_config[CLOCK_VRANGE1_HSI_PLL_32MHZ]);
}

int
main()
{
  configure_mcu_clocks();
  uart_init(115200);
  printf("Platform init complete, starting contiki init\n");

  clock_init();
  rtimer_init();
  leds_init();
  process_init();
#if WITH_SERIAL_LINE_INPUT
  uart_set_input(serial_line_input_byte);
  serial_line_init();
#endif
  process_start(&etimer_process, NULL);
  ctimer_init();
  autostart_start(autostart_processes);
  while(1) {
    do {
	// meant to do some sleeping here, if we want to save power...
	//
    } while(process_run() > 0);
  }
  return 0;
}
