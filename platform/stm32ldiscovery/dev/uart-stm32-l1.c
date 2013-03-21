#include <libopencm3/stm32/l1/gpio.h>

void uart_init_gpio(void)
{
        /* Setup GPIO pins for USART2 transmit. */
        gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO9);
        gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO10);
        /* Setup USART2 TX pin as alternate function. */
        gpio_set_af(GPIOA, GPIO_AF7, GPIO9);
        gpio_set_af(GPIOA, GPIO_AF7, GPIO10);

        /* USART lines should idle high */
        gpio_set(GPIOA, GPIO9);
        gpio_set(GPIOA, GPIO10);
}

