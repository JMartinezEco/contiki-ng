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
#include "uart0-arch.h"
#include "dev/leds.h"
#include "net/routing/routing.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"

#include "sys/log.h"
#define LOG_MODULE "Server"
#define LOG_LEVEL LOG_LEVEL_DBG

#define WITH_SERVER_REPLY 1
#define UDP_CLIENT_PORT 8765
#define UDP_SERVER_PORT 5678

#define SEND_INTERVAL (5 * CLOCK_SECOND)

#define RED 1
#define GREEN 2

static struct simple_udp_connection udp_conn;
int addresses[4];

unsigned char received_message_from_uart[200];
unsigned char processed_message_from_uart[200];
int end_of_string = 0;
int index = 0;

static struct etimer timer;

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
  leds_on(GREEN);
  printf("\n");
  printf("%s\n", data);
  printf("\n");
  leds_off(GREEN);
  // LOG_INFO("Received request '%.*s' from ", datalen, (char *)data);
  // LOG_INFO_6ADDR(sender_addr);
  // LOG_INFO_("\n");

  // // float rssi = get_value(RADIO_PARAM_LAST_RSSI);
  // radio_value_t radio_rssi;
  // NETSTACK_RADIO.get_value(RADIO_PARAM_LAST_RSSI, &radio_rssi);
  // printf("El RSSI medido es de: %i", radio_rssi);
  // printf("\n");
}

int uart_handler(unsigned char c)
{
  if (c == '\n')
  {
    end_of_string = 1;
    index = 0;
  }
  else
  {
    received_message_from_uart[index] = c;
    index++;
  }
  return 0;
}

void getNodeAddressBytes(unsigned char received_message_from_uart[200])
{

  for (int i = 0; i < 4; i++)
  {
    // Extract evey four bytes of the complete message
    int x = (int)received_message_from_uart[0 + i * 4] < 65 ? (int)received_message_from_uart[0 + i * 4] - 48 : (int)received_message_from_uart[0 + i * 4] - 55;
    int y = (int)received_message_from_uart[1 + i * 4] < 65 ? (int)received_message_from_uart[1 + i * 4] - 48 : (int)received_message_from_uart[1 + i * 4] - 55;
    int z = (int)received_message_from_uart[2 + i * 4] < 65 ? (int)received_message_from_uart[2 + i * 4] - 48 : (int)received_message_from_uart[2 + i * 4] - 55;
    int w = (int)received_message_from_uart[3 + i * 4] < 65 ? (int)received_message_from_uart[3 + i * 4] - 48 : (int)received_message_from_uart[3 + i * 4] - 55;

    // Combine hex chars to int
    addresses[i] = (x << 12) | (y << 8) | (z << 4) | w;

    // Clear aux variables
    x = 0;
    y = 0;
    z = 0;
    w = 0;
  }

  // Obtain len of the received message
  int received_message_from_uart_len = strlen((char *)received_message_from_uart);

  // Delete 16 first bytes from received_message_from_uart
  for (int i = 0; i < received_message_from_uart_len; i++)
  {
    processed_message_from_uart[i] = received_message_from_uart[i + 16];
  }
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
  uip_ipaddr_t dest_ipaddr;
  PROCESS_BEGIN();

  uart0_init();
  uart0_set_callback(uart_handler);

  leds_on(RED);
  leds_on(GREEN);
  etimer_set(&timer, 0.2 * CLOCK_SECOND);
  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
  leds_off(RED);
  leds_off(GREEN);

  while (1)
  {
    if (end_of_string == 1 && received_message_from_uart[0] != 0)
    {
      leds_on(RED);
      getNodeAddressBytes(received_message_from_uart);

      uip_ip6addr(&dest_ipaddr, 0xfd00, 0, 0, 0, (0x0200 + addresses[0]), addresses[1], addresses[2], addresses[3]);
      // Enviamos el mensaje de vuelta
      simple_udp_sendto(&udp_conn, processed_message_from_uart, 100, &dest_ipaddr);

      //  Se borra el buffer del mensaje UART
      index = 0;
      end_of_string = 0;
      memset(received_message_from_uart, 0, sizeof received_message_from_uart);
      memset(processed_message_from_uart, 0, sizeof processed_message_from_uart);
      leds_off(RED);
    }
    etimer_set(&timer, 0.1 * CLOCK_SECOND);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
  }
  PROCESS_END();
}