

#include "contiki.h"
#include <stdlib.h>
#include "net/routing/routing.h"
#include "random.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"


#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_DBG

#define UDP_CLIENT_PORT	8765
#define UDP_SERVER_PORT	5678

#define SEND_INTERVAL		  (5 * CLOCK_SECOND)

static struct simple_udp_connection udp_conn;
static struct etimer periodic_timer;

PROCESS(udp_server_process, "UDP server");
PROCESS(send_msg_process, "UDP server");
AUTOSTART_PROCESSES(&udp_server_process, &send_msg_process);


static void
udp_rx_callback(struct simple_udp_connection *c,
         const uip_ipaddr_t *sender_addr,
         uint16_t sender_port,
         const uip_ipaddr_t *receiver_addr,
         uint16_t receiver_port,
         const uint8_t *data,
         uint16_t datalen)
{
  LOG_INFO("Received response '%.*s' from ", datalen, (char *) data);
  LOG_INFO_6ADDR(sender_addr);
  LOG_INFO_("\n");
    // Estas dos de debajo para DALI
    // printf("%s", data);
    // memset((char*)data, 0, datalen );

  // float rssi = get_value(RADIO_PARAM_LAST_RSSI);
  radio_value_t radio_rssi; 
  NETSTACK_RADIO.get_value(RADIO_PARAM_LAST_RSSI, &radio_rssi);
  printf("El RSSI medido es de: %i", radio_rssi);
  printf("\n");
}




PROCESS_THREAD(udp_server_process, ev, data)
{
  PROCESS_BEGIN();

  /* Initialize DAG root */
  NETSTACK_ROUTING.root_start();

  /* Initialize UDP connection */
  simple_udp_register(&udp_conn, UDP_SERVER_PORT, NULL,
                      UDP_CLIENT_PORT, udp_rx_callback);

  PROCESS_END();
}


PROCESS_THREAD(send_msg_process, ev, data)
{
  
  static unsigned count;
  static char str[32];
  uip_ipaddr_t dest_ipaddr;
  LOG_INFO("%u", count);
  PROCESS_BEGIN();



  while(1) {
      etimer_set(&periodic_timer, CLOCK_SECOND);
      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
        uip_ip6addr(&dest_ipaddr,0xfd00,0,0,0,0x207,0x7,0x7,0x7);
        LOG_INFO("Sending request %u to ", count);
        LOG_INFO_6ADDR(&dest_ipaddr);
        LOG_INFO_("\n");
        snprintf(str, sizeof(str), "hello %d", count);
        simple_udp_sendto(&udp_conn, str, strlen(str), &dest_ipaddr);
        count++;

  }

  PROCESS_END();
}