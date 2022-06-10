#ifndef PROJECT_CONF_H_
#define PROJECT_CONF_H_

// Settings RF
#define RF_CONF_MODE RF_MODE_SUB_1_GHZ

// Perifericos
#define TI_SPI_CONF_SPI0_ENABLE 0
#define TI_SPI_CONF_SPI1_ENABLE 0
#define TI_SPI_CONF_SPI1_ENABLE 0
#define TI_I2C_CONF_I2C0_ENABLE 0
#define TI_NVS_CONF_NVS_INTERNAL_ENABLE 0
#define TI_NVS_CONF_NVS_EXTERNAL_ENABLE 0

// Settings packet size
// #define COOJA_RADIO_CONF_BUFSIZE 5000
// #define RF_CONF_RX_BUF_SIZE 800
#define PACKETBUF_CONF_SIZE 500
// #define UIP_CONF_BUFFER_SIZE 800
// #define SICSLOWPAN_CONF_MAC_MAX_PAYLOAD 1280

// Settings RPL
// #define COOJA_RADIO_CONF_BUFSIZE 1024
// #define PACKETBUF_CONF_SIZE 1024
#define RPL_CONF_DIS_INTERVAL (5 * CLOCK_SECOND)

// Settings bootloader
#define SET_CCFG_BL_CONFIG_BOOTLOADER_ENABLE 0xC5 // Enable ROM boot loader
#define SET_CCFG_BL_CONFIG_BL_LEVEL 0x1			  // Active high to open boot loader backdoor
#define SET_CCFG_BL_CONFIG_BL_PIN_NUMBER 0x08	  // DIO number for boot loader backdoor
#define SET_CCFG_BL_CONFIG_BL_ENABLE 0xC5		  // Enabled boot loader backdoor

// Ram optimization
// the number of packets in the link-layer queue. 4 is probably a lower bound for reasonable operation. As the traffic load increases, e.g. more frequent traffic or larger datagrams, you will need to increase this parameter.
#define QUEUEBUF_CONF_NUM 4

// the number of entries in the neighbor table. A value greater than the maximum network density is safe. A value lower than that will also work, as the neighbor table will automatically focus on relevant neighbors. But too low values will result in degraded performance.
#define NBR_TABLE_CONF_MAX_NEIGHBORS 2

// the number of routing entries, i.e., in RPL non-storing mode, the number of links in the routing graph, and in storing mode, the number of routing table elements. At the network root, this must be set to the maximum network size. In non-storing mode, other nodes can set this parameter to 0. In storing mode, it is recommended for all nodes to also provision enough entries for each node in the network.
#define NETSTACK_MAX_ROUTE_ENTRIES 30

// As mentioned above. Disable if no fragmentation is needed.
#define SICSLOWPAN_CONF_FRAG 1

// Settings Logs
// #define LOG_CONF_LEVEL_RPL 0
// #define LOG_CONF_LEVEL_TCPIP 0
// #define LOG_CONF_LEVEL_IPV6 0
// #define LOG_CONF_LEVEL_6LOWPAN 0
// #define LOG_CONF_LEVEL_MAC 0
// #define LOG_CONF_LEVEL_FRAMER 0
// #define TSCH_LOG_CONF_PER_SLOT 0
// #define LOG_CONF_LEVEL_MAIN LOG_LEVEL_DBG

#endif
/* PROJECT_CONF_H_ */