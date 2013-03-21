#include <libopencm3/stm32/f1/gpio.h>
#include "uart-stm32-platform.h"
void uart_init_gpio(void)
{
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

