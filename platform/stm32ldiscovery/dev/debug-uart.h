/**
 * USART driver for STM32xx w/ libopencm3 peripheral lib
 * (c) 2012 Blessed Contraptions
 * Jeff Ciesielski <jeff.ciesielski@gmail.com>
 */

#ifndef _UART_H_
#define _UART_H_

int uart_putchar(char c);
int uart_getchar(char *c);
void uart_init(int baud);

#endif
