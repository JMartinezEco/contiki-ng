#include "contiki.h"
#include "net/routing/routing.h"
#include "random.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include "uart0-arch.h"
// #include "cc26xx-uart.h"
#include "dev/serial-line.h"

#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

#define WITH_SERVER_REPLY  1
#define UDP_CLIENT_PORT	8765
#define UDP_SERVER_PORT	5678

#define SEND_INTERVAL		  (1 * CLOCK_SECOND)

// static struct simple_udp_connection udp_conn;

unsigned char str[2];

/*---------------------------------------------------------------------------*/
PROCESS(udp_client_process, "UDP client");
AUTOSTART_PROCESSES(&udp_client_process);
/*---------------------------------------------------------------------------*/
// static void
// udp_rx_callback(struct simple_udp_connection *c,
//          const uip_ipaddr_t *sender_addr,
//          uint16_t sender_port,
//          const uip_ipaddr_t *receiver_addr,
//          uint16_t receiver_port,
//          const uint8_t *data,
//          uint16_t datalen)
// {

//   LOG_INFO("Received response '%.*s' from ", datalen, (char *) data);
//   LOG_INFO_6ADDR(sender_addr);


// #if LLSEC802154_CONF_ENABLED
//   LOG_INFO_(" LLSEC LV:%d", uipbuf_get_attr(UIPBUF_ATTR_LLSEC_LEVEL));
// #endif
//   LOG_INFO_("\n");

// }
int uart_handler(unsigned char c){
    str[0] = c;
    return 0;
}




/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_client_process, ev, data)
{
  static struct etimer periodic_timer;
  // int state = 0;
  // static char str[32];
  // uip_ipaddr_t dest_ipaddr;
  // char * serial_msg_recv = NULL;

  PROCESS_BEGIN();
  uart0_init();
  // serial_line_init();
  // uart0_set_input(serial_line_input_byte);
  uart0_set_callback (uart_handler);

  // cc26xx_uart_set_input(serial_line_input_byte);


  /* Initialize UDP connection */
  /*simple_udp_register(&udp_conn, UDP_CLIENT_PORT, NULL,
                      UDP_SERVER_PORT, udp_rx_callback);*/
  
  // uart0_init();
  // uart0_set_callback(uart0_callback);                    

  etimer_set(&periodic_timer, random_rand() % SEND_INTERVAL);
  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
    // PROCESS_WAIT_EVENT();
    // PROCESS_YIELD();
    printf("El mensaje: ");
    printf("%s\n", str);
  //   // Aqui se debe leer el UART

    // if (ev == serial_line_event_message ) {
    //   printf("Tenemos mensaje\n");
    // }
  //   // if(NETSTACK_ROUTING.node_is_reachable() && NETSTACK_ROUTING.get_root_ipaddr(&dest_ipaddr)) {

  //   //   /* Send to DAG root */
  //   //   // Si se ha encontrado el root se envía el mensaje obtenido por el UART
  //   //   // snprintf(str, sizeof(str), "%d", state);
  //   //   // simple_udp_sendto(&udp_conn, str, strlen(str), &dest_ipaddr);
      
  //   // } else {

  //   //   // LOG_INFO("Not reachable yet\n");

  //   // }

    etimer_set(&periodic_timer, CLOCK_SECOND * 1);

  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
