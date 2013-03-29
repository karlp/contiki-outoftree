#include <errno.h>
#include <stdio.h>

#include <sys/energest.h>
#include <lib/ringbuf.h>

#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/nvic.h>
#include "uart-stm32-platform.h"

static struct ringbuf ringbuf_rx, ringbuf_tx;
static uint8_t buf_rx[128];
static uint8_t buf_tx[128];

void uart_init(int baud)
{
	ringbuf_init(&ringbuf_rx, buf_rx, sizeof(buf_rx));
	ringbuf_init(&ringbuf_tx, buf_tx, sizeof(buf_tx));
	uart_init_gpio();

	nvic_enable_irq(NVIC_USART1_IRQ);
	nvic_set_priority(NVIC_USART1_IRQ, 2);

	usart_set_baudrate(USART1, baud);
	usart_set_databits(USART1, 8);
	usart_set_parity(USART1, USART_PARITY_NONE);
	usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);
	usart_set_stopbits(USART1, USART_CR2_STOPBITS_1);
	usart_set_mode(USART1, USART_MODE_TX_RX);
	usart_enable_rx_interrupt(USART1);
	usart_enable(USART1);

	/* This ensures stdio doesn't use its own buffers */
	setvbuf(stdin, NULL, _IONBF, 0);
	setvbuf(stdout, NULL, _IONBF, 0);
}

int uart_putchar(char c)
{
	if( c == '\n')
		uart_putchar('\r');
	if (ringbuf_elements(&ringbuf_tx) < ringbuf_size(&ringbuf_tx)) {
		ringbuf_put(&ringbuf_tx, c);
	} else {
		return -ENOMEM;
	}

	usart_enable_tx_interrupt(USART1);

	return 0;
}

int uart_getchar(char *c)
{
	if (ringbuf_elements(&ringbuf_rx) > 0) {
		*c = ringbuf_get(&ringbuf_rx);
		return 0;
	} else {
		return -ENODATA;
	}
}

void usart1_isr(void)
{
	ENERGEST_ON(ENERGEST_TYPE_IRQ);
	char c;
	if (usart_get_flag(USART1, USART_SR_TXE)) {
		if (ringbuf_elements(&ringbuf_tx) > 0) {
			c = ringbuf_get(&ringbuf_tx);
			usart_send(USART1, (uint16_t)c);
		} else {
			usart_disable_tx_interrupt(USART1);
		}
	}


	if (usart_get_flag(USART1, USART_SR_RXNE)) {
		if (ringbuf_elements(&ringbuf_rx) < ringbuf_size(&ringbuf_rx)) {
			c = usart_recv(USART1);
			ringbuf_put(&ringbuf_rx, c);
		}
	}
	ENERGEST_OFF(ENERGEST_TYPE_IRQ);
}
