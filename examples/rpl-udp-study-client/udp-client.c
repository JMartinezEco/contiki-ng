#include "contiki.h"
// #include "uart0-arch.h"
// #include "dev/serial-line.h"
#include "dev/leds.h"
#include "net/routing/routing.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"

#include "sys/log.h"
#define LOG_MODULE "Client"
#define LOG_LEVEL LOG_LEVEL_DBG

#define UDP_CLIENT_PORT 8765
#define UDP_SERVER_PORT 5678

#define RED 1
#define GREEN 2

static struct simple_udp_connection udp_conn;

static struct etimer timer;

// IP del servidor que nos ha enviado el mensaje
uip_ipaddr_t dest_ipaddr;

/*---------------------------------------------------------------------------*/
PROCESS(udp_client_process, "UDP client");
AUTOSTART_PROCESSES(&udp_client_process);
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
  LOG_INFO("Received request from ");
  LOG_INFO_6ADDR(sender_addr);
  LOG_INFO("Sending response.\n");
  simple_udp_sendto(&udp_conn, "Recibido, gracias!", 20, sender_addr);
}

PROCESS_THREAD(udp_client_process, ev, data)
{

  PROCESS_BEGIN();

  simple_udp_register(&udp_conn, UDP_CLIENT_PORT, NULL,
                      UDP_SERVER_PORT, udp_rx_callback);
  leds_on(RED);
  leds_on(GREEN);
  etimer_set(&timer, 0.2 * CLOCK_SECOND);
  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
  leds_off(RED);
  leds_off(GREEN);

  // etimer_set(&timer, 0.5 * CLOCK_SECOND);
  // PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));

  PROCESS_END();
}
