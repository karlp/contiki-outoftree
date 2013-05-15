#include "contiki-conf.h"
#include "debug-uart-arch.h"
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

#if DEBUG_UART_CONF == 1
static void uart_init_gpio_usart1(void)
{
	/* Enable clocks for USART1 (and the gpios it's connected to) */
	rcc_peripheral_enable_clock(&RCC_AHBENR, RCC_AHBENR_GPIOAEN);
	rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_USART1EN);
	/* Setup USART1 pins to Alternate Function */
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO9 | GPIO10);
	/* Setup Alternate Function 7 - usart */
	gpio_set_af(GPIOA, GPIO_AF7, GPIO9 | GPIO10);

	/* USART lines should idle high */
	gpio_set(GPIOA, GPIO9 | GPIO10);
}
#endif

#if DEBUG_UART_CONF == 2
static void uart_init_gpio_usart2(void)
{
	/* Enable clocks for USART2 (and the gpios it's connected to) */
	rcc_peripheral_enable_clock(&RCC_AHBENR, RCC_AHBENR_GPIOAEN);
	rcc_peripheral_enable_clock(&RCC_APB1ENR, RCC_APB1ENR_USART2EN);
	/* Setup USART2 pins to Alternate Function */
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO2 | GPIO3);
	/* Setup Alternate Function 7 - usart */
	gpio_set_af(GPIOA, GPIO_AF7, GPIO2 | GPIO3);

	/* USART lines should idle high */
	gpio_set(GPIOA, GPIO2 | GPIO3);
}
#endif

void uart_init_arch(void)
{
	// Example platform shows all the options ;)
#if DEBUG_UART_CONF == 1
	uart_init_gpio_usart1();
#elif DEBUG_UART_CONF == 2
	uart_init_gpio_usart2();
#else
#error No support in this example platform for other uarts!
#endif
}

