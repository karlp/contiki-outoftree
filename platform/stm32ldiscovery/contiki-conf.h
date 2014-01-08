#ifndef __CONTIKI_CONF_H__CDBB4VIH3I__
#define __CONTIKI_CONF_H__CDBB4VIH3I__

#include <stdint.h>

#define CCIF
#define CLIF

#define WITH_UIP 1
#define WITH_UIP6 1
#define WITH_ASCII 1
#define WITH_SERIAL_LINE_INPUT 1

#define DEBUG_UART_CONF 2

#define CLOCK_CONF_SECOND 1000

typedef unsigned int clock_time_t;
typedef unsigned int uip_stats_t;

#ifndef BV
#define BV(x) (1<<(x))
#endif

/// From seedeye
#define RF_CHANNEL                              12
#define IEEE802154_CONF_PANID			0xcafe
#define ADD_RSSI_AND_LQI_TO_PACKET		1

#define PROFILE_CONF_ON 0
#ifndef ENERGEST_CONF_ON
#define ENERGEST_CONF_ON 1
#endif /* ENERGEST_CONF_ON */

#ifdef WITH_UIP6
#define NETSTACK_CONF_NETWORK                   sicslowpan_driver
#define NETSTACK_CONF_FRAMER                    framer_802154
#define NETSTACK_CONF_MAC                       nullmac_driver
#define NETSTACK_CONF_RDC                       nullrdc_driver
#define NETSTACK_CONF_RADIO                     mrf24j40_driver
#define RIMEADDR_CONF_SIZE                      8
#else
#define NETSTACK_CONF_NETWORK                   rime_driver
#define NETSTACK_CONF_FRAMER                    framer_802154
#define NETSTACK_CONF_MAC                       nullmac_driver
#define NETSTACK_CONF_RDC                       nullrdc_driver
#define NETSTACK_CONF_RADIO                     mrf24j40_driver
#define RIMEADDR_CONF_SIZE                      2
#endif


#ifdef WITH_UIP6
#define UIP_CONF_ROUTER                         1
#ifndef UIP_CONF_IPV6_RPL
#define UIP_CONF_IPV6_RPL                       1
#endif /* UIP_CONF_IPV6_RPL */

/* IPv6 configuration options */
#define UIP_CONF_IPV6                           1
#define UIP_CONF_DS6_NBR_NBU                    20 /* number of neighbors */
#define UIP_CONF_DS6_ROUTE_NBU                  20 /* number of routes */
#define UIP_CONF_ND6_SEND_RA                    0
#define UIP_CONF_ND6_REACHABLE_TIME             600000
#define UIP_CONF_ND6_RETRANS_TIMER              10000


#define UIP_CONF_BUFFER_SIZE                    240

#ifndef QUEUEBUF_CONF_NUM
#define QUEUEBUF_CONF_NUM                       16
#endif /* QUEUEBUF_CONF_NUM */

/* UDP configuration options */
#define UIP_CONF_UDP                            1
#define UIP_CONF_UDP_CHECKSUMS                  1
#define UIP_CONF_UDP_CONNS                      10

/* 6lowpan options (for ipv6) */
#define SICSLOWPAN_CONF_COMPRESSION             SICSLOWPAN_COMPRESSION_HC06
#define SICSLOWPAN_CONF_MAX_ADDR_CONTEXTS       2
#define SICSLOWPAN_CONF_COMPRESSION_THRESHOLD   63
#ifndef SICSLOWPAN_CONF_FRAG
#define SICSLOWPAN_CONF_FRAG                    1
#define SICSLOWPAN_CONF_MAXAGE                  8
#endif /* SICSLOWPAN_CONF_FRAG */

// Configure NullRDC to do the right thing, if it's selected
#define NULLRDC_802154_AUTOACK			1
#define NULLRDC_802154_AUTOACK_HW		1

/* General configuration options */
#define UIP_CONF_STATISTICS                     0
//#define UIP_CONF_LOGGING                        0
#define UIP_CONF_LOGGING                        1
#define UIP_CONF_BROADCAST                      1
#define UIP_CONF_LLH_LEN                        0
#define UIP_CONF_LL_802154                      1
#endif

//// END OF SEEDEYE

//#define UIP_CONF_TCP_FORWARD 1

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
