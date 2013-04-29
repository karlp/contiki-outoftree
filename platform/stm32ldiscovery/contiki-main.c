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

#include <libopencm3/stm32/l1/rcc.h>


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
