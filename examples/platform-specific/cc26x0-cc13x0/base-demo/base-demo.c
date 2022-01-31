/*
 * Copyright (c) 2014, Texas Instruments Incorporated - http://www.ti.com/
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*---------------------------------------------------------------------------*/
/**
 * \addtogroup cc26xx-platforms
 * @{
 *
 * \defgroup cc26xx-examples CC26xx Example Projects
 *
 * Example projects for CC26xx-based platforms.
 * @{
 *
 * \defgroup cc26xx-demo CC13xx/CC26xx Base Demo
 *
 *   Example project demonstrating the CC13xx/CC26xx platforms
 *
 *   This example was developed for the `cc26x0-cc13x0` platform and will work
 *   for the following boards:
 *   - cc26x0-cc13x0: SmartRF06EB + CC13xx/CC26xx EM
 *   - CC2650 and CC1350 SensorTag
 *   - CC1310, CC1350, CC2650 LaunchPads
 *
 *   This example was subsequently modified to also work on the `simplelink`
 *   platform and the intention is for it to work on all supported boards.
 *
 *   This is an IPv6/RPL-enabled example. Thus, if you have a border router in
 *   your installation (same RDC layer, same PAN ID and RF channel), you should
 *   be able to ping6 this demo node.
 *
 *   This example also demonstrates CC26xx BLE operation. The process starts
 *   the BLE beacon daemon (implemented in the RF driver). The daemon will
 *   send out a BLE beacon periodically. Use any BLE-enabled application (e.g.
 *   LightBlue on OS X or the TI BLE Multitool smartphone app) and after a few
 *   seconds the cc26xx device will be discovered.
 *
 * - etimer/clock : Every CC26XX_DEMO_LOOP_INTERVAL clock ticks the LED defined
 *                  as CC26XX_DEMO_LEDS_PERIODIC will toggle and the device
 *                  will print out readings from some supported sensors
 * - sensors      : Some sensortag sensors are read asynchronously (see sensor
 *                  documentation). For those, this example will print out
 *                  readings in a staggered fashion at a random interval
 * - Buttons      : CC26XX_DEMO_TRIGGER_1 button will toggle CC26XX_DEMO_LEDS_BUTTON
 *                - CC26XX_DEMO_TRIGGER_2 turns on LEDS_REBOOT and causes a
 *                  watchdog reboot
 *                - The remaining buttons will just print something
 *                - The example also shows how to retrieve the duration of a
 *                  button press (in ticks). The driver will generate a
 *                  sensors_changed event upon button release
 * - Reed Relay   : Will toggle the sensortag buzzer on/off
 *
 * @{
 *
 * \file
 *     Example demonstrating the cc26xx platforms
 */
#include "contiki.h"
#include "sys/etimer.h"
#include "sys/ctimer.h"
#include "dev/leds.h"
#include "dev/watchdog.h"
#include "dev/button-hal.h"
#include "random.h"
#include "button-sensor.h"
#include "batmon-sensor.h"
#include "board-peripherals.h"
#include "target-conf.h"

#include <stdio.h>
#include <stdint.h>
/*---------------------------------------------------------------------------*/
#define CC26XX_DEMO_LOOP_INTERVAL       (CLOCK_SECOND * 20)
#define CC26XX_DEMO_LEDS_PERIODIC       LEDS_YELLOW
#define CC26XX_DEMO_LEDS_BUTTON         LEDS_RED
#define CC26XX_DEMO_LEDS_REBOOT         LEDS_ALL
/*---------------------------------------------------------------------------*/
static struct etimer et;
/*---------------------------------------------------------------------------*/
PROCESS(base_demo_process, "CC13xx/CC26xx base demo process");
AUTOSTART_PROCESSES(&base_demo_process);
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(base_demo_process, ev, data)
{

  PROCESS_BEGIN();

  printf("CC13xx/CC26xx base demo\n");


#if CONTIKI_TARGET_CC26X0_CC13X0
  /* Init the BLE advertisement daemon */
  rf_ble_beacond_config(0, BOARD_STRING);
  rf_ble_beacond_start();
#endif

  etimer_set(&et, CC26XX_DEMO_LOOP_INTERVAL);

  while(1) {

    PROCESS_YIELD();

    if(ev == PROCESS_EVENT_TIMER) {
      if(data == &et) {
        leds_toggle(CC26XX_DEMO_LEDS_PERIODIC);

        etimer_set(&et, CC26XX_DEMO_LOOP_INTERVAL);
      }
    } else if(ev == button_hal_periodic_event) {
      button_hal_button_t *button = data;

      printf("%s periodic event, duration %d seconds\n",
             BUTTON_HAL_GET_DESCRIPTION(button),
             button->press_duration_seconds);
    } else if(ev == button_hal_press_event) {
      button_hal_button_t *btn = (button_hal_button_t *)data;

      printf("%s press event\n", BUTTON_HAL_GET_DESCRIPTION(btn));

    } else if(ev == button_hal_release_event) {
      button_hal_button_t *btn = (button_hal_button_t *)data;

      printf("%s release event\n", BUTTON_HAL_GET_DESCRIPTION(btn));

    }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
/**
 * @}
 * @}
 * @}
 */
