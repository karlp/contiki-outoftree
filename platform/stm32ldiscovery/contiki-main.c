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

#include <contiki-net.h>
#include <net/netstack.h>
#include <net/queuebuf.h>
#include <net/mac/frame802154.h>

#if WITH_UIP6
#include "net/uip-ds6.h"
#endif /* WITH_UIP6 */

#include "contiki-conf.h"
#include <libopencm3/stm32/rcc.h>

#define DEBUG 1
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif


static void
configure_mcu_clocks(void)
{
	rcc_clock_setup_pll(&clock_config[CLOCK_VRANGE1_HSI_PLL_32MHZ]);
}

void
init_net(uint8_t node_id)
{
  uint16_t shortaddr;
  uint64_t longaddr;
  rimeaddr_t addr;
#if WITH_UIP6
  uip_ds6_addr_t *lladdr;
  uip_ipaddr_t ipaddr;
#endif
  
  uint8_t i;

  memset(&shortaddr, 0, sizeof(shortaddr));
  memset(&longaddr, 0, sizeof(longaddr));
  *((uint8_t *)&shortaddr) = node_id >> 8;
  *((uint8_t *)&shortaddr + 1) = node_id;
  *((uint8_t *)&longaddr) = node_id >> 8;
  *((uint8_t *)&longaddr + 1) = node_id;
  for(i = 2; i < sizeof(longaddr); ++i) {
    ((uint8_t *)&longaddr)[i] = random_rand();
  }
  
  PRINTF("SHORT MAC ADDRESS %02x:%02x\n",
         *((uint8_t *) & shortaddr), *((uint8_t *) & shortaddr + 1));

  PRINTF("EXTENDED MAC ADDRESS %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n",
         *((uint8_t *)&longaddr),
         *((uint8_t *)&longaddr + 1),
         *((uint8_t *)&longaddr + 2),
         *((uint8_t *)&longaddr + 3),
         *((uint8_t *)&longaddr + 4),
         *((uint8_t *)&longaddr + 5),
         *((uint8_t *)&longaddr + 6),
         *((uint8_t *)&longaddr + 7));

  memset(&addr, 0, sizeof(rimeaddr_t));

  for(i = 0; i < sizeof(addr.u8); ++i) {
    addr.u8[i] = ((uint8_t *)&longaddr)[i];
  }

  rimeaddr_set_node_addr(&addr);
  
  PRINTF("Rime started with address: ");
  for(i = 0; i < sizeof(addr.u8) - 1; ++i) {
    PRINTF("%d.", addr.u8[i]);
  }
  PRINTF("%d\n", addr.u8[i]);

  queuebuf_init();

  NETSTACK_RADIO.init();
  
  mrf24j40_set_channel(RF_CHANNEL);
  mrf24j40_set_panid(IEEE802154_PANID);
  mrf24j40_set_short_mac_addr(shortaddr);
  mrf24j40_set_extended_mac_addr(longaddr);

  NETSTACK_RDC.init();
  NETSTACK_MAC.init();
  NETSTACK_NETWORK.init();

  PRINTF("%s %s, channel check rate %d Hz, radio channel %u\n",
         NETSTACK_MAC.name, NETSTACK_RDC.name,
         CLOCK_SECOND / (NETSTACK_RDC.channel_check_interval() == 0 ? 1 :
                         NETSTACK_RDC.channel_check_interval()), RF_CHANNEL);

#if WITH_UIP6

#if RIMEADDR_CONF_SIZE == 2
  memset(&uip_lladdr.addr, 0, sizeof(uip_lladdr.addr));
  uip_lladdr.addr[3] = 0xff;
  uip_lladdr.addr[4]= 0xfe;
  memcpy(&uip_lladdr.addr[6], &shortaddr, sizeof(shortaddr));
#else
  memcpy(&uip_lladdr.addr, &longaddr, sizeof(uip_lladdr.addr));
#endif

  process_start(&tcpip_process, NULL);

  lladdr = uip_ds6_get_link_local(-1);

  PRINTF("Tentative link-local IPv6 address ");

  for(i = 0; i < 7; ++i) {
    PRINTF("%02x%02x:", lladdr->ipaddr.u8[i * 2], lladdr->ipaddr.u8[i * 2 + 1]);
  }

  PRINTF("%02x%02x\n", lladdr->ipaddr.u8[14], lladdr->ipaddr.u8[15]);

  if(!UIP_CONF_IPV6_RPL) {
    uip_ip6addr(&ipaddr, 0x2001, 0x1418, 0x100, 0x823c, 0, 0, 0, 0);
    uip_ds6_set_addr_iid(&ipaddr, &uip_lladdr);
    uip_ds6_addr_add(&ipaddr, 0, ADDR_TENTATIVE);

    PRINTF("Tentative global IPv6 address ");

    for(i = 0; i < 7; ++i) {
      PRINTF("%02x%02x:", ipaddr.u8[i * 2], ipaddr.u8[i * 2 + 1]);
    }

    PRINTF("%02x%02x\n", ipaddr.u8[7 * 2], ipaddr.u8[7 * 2 + 1]);
  }
#endif
}



int
main()
{
  configure_mcu_clocks();
  uart_init(115200);
  printf("Platform init complete, starting contiki init\n");

  clock_init();
  rtimer_init();
  leds_init();
  init_net(0x88);
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
