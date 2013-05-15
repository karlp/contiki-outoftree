#ifndef __CONTIKI_CONF_H__CDBB4VIH3I__
#define __CONTIKI_CONF_H__CDBB4VIH3I__

#include <stdint.h>

#define CCIF
#define CLIF

#define WITH_UIP 1
#define WITH_ASCII 1
#define WITH_SERIAL_LINE_INPUT 1

#define DEBUG_UART_CONF 2

#define CLOCK_CONF_SECOND 1000

typedef unsigned int clock_time_t;
typedef unsigned int uip_stats_t;

#ifndef BV
#define BV(x) (1<<(x))
#endif

/* uIP configuration */
#define UIP_CONF_LLH_LEN         0
#define UIP_CONF_BROADCAST       1
#define UIP_CONF_LOGGING 1
#define UIP_CONF_BUFFER_SIZE 116

#define UIP_CONF_TCP_FORWARD 1

/* Prefix for relocation sections in ELF files */
#define REL_SECT_PREFIX ".rel"

#define CC_BYTE_ALIGNED __attribute__ ((packed, aligned(1)))

#define USB_EP1_SIZE 64
#define USB_EP2_SIZE 64

#define RAND_MAX 0x7fff

/* Not everyone runs AHB at Sysclk speed */
#ifndef AHB_SPEED
#define AHB_SPEED MCK
#endif

#endif /* __CONTIKI_CONF_H__CDBB4VIH3I__ */
