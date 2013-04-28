#include <stdint.h>
#include <stdio.h>
#include <debug-uart.h>
#include <sys/process.h>
#include <sys/procinit.h>
#include <etimer.h>
#include <sys/autostart.h>
#include <clock.h>

#include <libopencm3/stm32/l1/rcc.h>

#include "dev/leds.h"

static void
configure_mcu_clocks(void)
{
	rcc_clock_setup_pll(&clock_vrange1_config[CLOCK_VRANGE1_HSI_PLL_32MHZ]);

	/* Enable clocks for USART1 (and the gpios it's connected to) */
	rcc_peripheral_enable_clock(&RCC_AHBENR, RCC_AHBENR_GPIOAEN);
	rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_USART1EN);
}

int
main()
{
  configure_mcu_clocks();
  uart_init(115200);
  printf("Platform init complete\n");

  clock_init();
  leds_init();
  process_init();
  process_start(&etimer_process, NULL);
  ctimer_init();
  autostart_start(autostart_processes);
  while(1) {
    do {
    } while(process_run() > 0);
  }
  return 0;
}
