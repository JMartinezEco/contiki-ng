/*
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */

#include "contiki.h"
#include "net/routing/routing.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"

#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

#define WITH_SERVER_REPLY  1
#define UDP_CLIENT_PORT	8765
#define UDP_SERVER_PORT	5678

#define SEND_INTERVAL		  (5 * CLOCK_SECOND)

static struct simple_udp_connection udp_conn;

PROCESS(udp_server_process, "UDP server");
PROCESS(send_msg_process, "UDP message");
AUTOSTART_PROCESSES(&udp_server_process, &send_msg_process);
/*---------------------------------------------------------------------------*/
static void
udp_rx_callback(struct simple_udp_connection *c,
         const uip_ipaddr_t *sender_addr,
         uint16_t sender_port,
         const uip_ipaddr_t *receiver_addr,
         uint16_t receiver_port,
         const uint8_t *data,
         uint16_t datalen)
{
  LOG_INFO("Received request '%.*s' from ", datalen, (char *) data);
  LOG_INFO_6ADDR(sender_addr);
  LOG_INFO_("\n");

  // float rssi = get_value(RADIO_PARAM_LAST_RSSI);
  radio_value_t radio_rssi; 
  NETSTACK_RADIO.get_value(RADIO_PARAM_LAST_RSSI, &radio_rssi);
  printf("El RSSI medido es de: %i", radio_rssi);
  printf("\n");

}




/*---------------------------------------------------------------------------*/
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
/*---------------------------------------------------------------------------*/


PROCESS_THREAD(send_msg_process, ev, data)
{
  static struct etimer periodic_timer;
  static unsigned count = 2;
  static char str[300];
  uip_ipaddr_t dest_ipaddr;
  PROCESS_BEGIN();


  etimer_set(&periodic_timer, SEND_INTERVAL);
  while(1) {
      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
        // uip_ip6addr(&dest_ipaddr,0xfd00,0,0,0,0x200+count,0x0+count,0x0+count,0x0+count);
        if (count%2==0){

        uip_ip6addr(&dest_ipaddr,0xfd00,0,0,0,0x0212,0x4B00,0x141F,0x9084);
        }else{

        uip_ip6addr(&dest_ipaddr,0xfd00,0,0,0,0x0212,0x4B00,0x2656,0x7739);
        }
        LOG_INFO("Sending request %u to ", count);
        LOG_INFO_6ADDR(&dest_ipaddr);
        LOG_INFO_("\n");
        // snprintf(str, sizeof(str), "{id: '234', count: %d, function: 'getAllData', dimming: '0.4'}", count);
        snprintf(str, sizeof(str), "hello: %d", count);
        simple_udp_sendto(&udp_conn, str, strlen(str), &dest_ipaddr);
        count++;
        if (count == 10) {
          count = 2;
        }
  etimer_set(&periodic_timer, SEND_INTERVAL);
 
  }

  PROCESS_END();
}