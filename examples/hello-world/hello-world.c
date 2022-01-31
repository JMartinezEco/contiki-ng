
#include "contiki.h"
#include "simple-udp.h"
#include "sys/log.h"
#define LOG_MODULE "Main"
#define LOG_LEVEL LOG_LEVEL_MAIN

static struct simple_udp_connection udp_conn;
static void
udp_rx_callback(struct simple_udp_connection *c,
         const uip_ipaddr_t *sender_addr,
         uint16_t sender_port,
         const uip_ipaddr_t *receiver_addr,
         uint16_t receiver_port,
         const uint8_t *data,
         uint16_t datalen)
{
  printf("Se ha recibido un mensaje");	
  printf("\n");
}



/*---------------------------------------------------------------------------*/
PROCESS(hello_world_process, "Hello world process");
AUTOSTART_PROCESSES(&hello_world_process);
/*---------------------------------------------------------------------------*/


PROCESS_THREAD(hello_world_process, ev, data)
{
  static struct etimer timer;

  PROCESS_BEGIN();

  uip_ipaddr_t destination_ipaddr;
  uip_ip6addr(&destination_ipaddr, 0xFF02,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0001);

  if(simple_udp_register(&udp_conn, 20000, NULL, 20000, udp_rx_callback)){
    printf("Se ha abierto el puerto UDP correctamente\n");
  }else{
    printf("No se ha podido abrir el puerto UDP\n");
  }

  etimer_set(&timer, CLOCK_SECOND * 1);

  uint8_t payload[2] = { 0x1, 0x2 };
  simple_udp_sendto(&udp_conn, payload, 2, &destination_ipaddr);
  while(1) {
    
    printf("DIOS FUNCIONA fenomenalmente bien");	
    printf("\n");

    simple_udp_send (&udp_conn, payload, 2);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
    etimer_reset(&timer);
  }



  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
