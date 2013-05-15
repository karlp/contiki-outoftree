#include "contiki-conf.h"
#include "debug-uart-arch.h"
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

#if DEBUG_UART_CONF == 1

void uart_init_gpio_usart1(void)
{
	/* Enable clocks for USART1 (and the gpios it's connected to) */
	rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_IOPAEN);
	rcc_peripheral_enable_clock(&RCC_APB1ENR, RCC_APB1ENR_USART1EN);
	gpio_set_mode(GPIOA,
		GPIO_MODE_OUTPUT_50_MHZ,
		GPIO_CNF_OUTPUT_ALTFN_PUSHPULL,
		GPIO9);

	gpio_set_mode(GPIOA,
		GPIO_MODE_INPUT,
		GPIO_CNF_INPUT_FLOAT,
		GPIO10);

	/* USART lines should idle high */
	gpio_set(GPIOA, GPIO9);
	gpio_set(GPIOA, GPIO10);
}
#endif

#if DEBUG_UART_CONF == 2

void uart_init_gpio_usart2(void)
{
	/* Enable clocks for USART2 (and the gpios it's connected to) */
	rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_IOPAEN);
	rcc_peripheral_enable_clock(&RCC_APB1ENR, RCC_APB1ENR_USART2EN);

	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
		GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO2);
	gpio_set_mode(GPIOA, GPIO_MODE_INPUT,
		GPIO_CNF_INPUT_FLOAT, GPIO3);

	/* USART lines should idle high */
	gpio_set(GPIOA, GPIO2);
	gpio_set(GPIOA, GPIO3);
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


