#include "contiki.h"
#include "uart0-arch.h"
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
unsigned char received_message_from_uart[200];
int end_of_string = 0;
int index = 0;

static struct etimer timer;

// IP del servidor que nos ha enviado el mensaje
uip_ipaddr_t dest_ipaddr;

/*---------------------------------------------------------------------------*/
PROCESS(udp_client_process, "UDP client");
PROCESS(udp_tracking_process, "UDP tracking");
AUTOSTART_PROCESSES(&udp_client_process, &udp_tracking_process);
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

  // Almacenamos la IP que nos ha enviado el mensaje
  dest_ipaddr = *sender_addr;

  printf("%s\n", data);
  leds_off(GREEN);
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

PROCESS_THREAD(udp_client_process, ev, data)
{

  PROCESS_BEGIN();

  uart0_init();
  uart0_set_callback(uart_handler);

  simple_udp_register(&udp_conn, UDP_CLIENT_PORT, NULL,
                      UDP_SERVER_PORT, udp_rx_callback);
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

      // Enviamos el mensaje de vuelta
      simple_udp_sendto(&udp_conn, received_message_from_uart, 200, &dest_ipaddr);

      //  Se borra el buffer del mensaje UART
      index = 0;
      end_of_string = 0;
      leds_off(RED);
      memset(received_message_from_uart, 0, sizeof received_message_from_uart);
    }
    etimer_set(&timer, 0.5 * CLOCK_SECOND);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
  }
  PROCESS_END();
}

PROCESS_THREAD(udp_tracking_process, ev, data)
{

  PROCESS_BEGIN();

  simple_udp_register(&udp_conn, UDP_CLIENT_PORT, NULL,
                      UDP_SERVER_PORT, udp_rx_callback);

  while (1)
  {
    if (NETSTACK_ROUTING.get_root_ipaddr(&dest_ipaddr))
    {
      static char str[32];
      // snprintf(str, sizeof(str), "Tracking &dest_ipaddr", count);
      // Enviamos el mensaje de vuelta
      simple_udp_sendto(&udp_conn, "Tracking", 20, &dest_ipaddr);
    }
    etimer_set(&timer, (10 * CLOCK_SECOND));
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
  }
  PROCESS_END();
}