

#include "chconf.h"
#include "halconf.h"
#include "rt_test_root.h"
#include "oslib_test_root.h"
#include <string.h>

/*
 * Green LED blinker thread, times are in milliseconds.
 */
static THD_WORKING_AREA(waThread1, 128);
static THD_FUNCTION(Thread1, arg) {

  (void)arg;
  chRegSetThreadName("blinker");
  while (true) {
    palToggleLine(LINE_LED_GREEN);
    chThdSleepMilliseconds(100);
  }
}

static THD_WORKING_AREA(waThread2, 128);
static THD_FUNCTION(Thread2, arg) {

  (void)arg;
  chRegSetThreadName("PuertoSerie");
  /*
   * Activates the serial driver 2 using the driver default configuration.
   */
  sdStart(&LPSD1, NULL);

  while (true) {
    sdWrite(&LPSD1, (uint8_t*)"Hello world", strlen("Hello world"));
    chThdSleepMilliseconds(500);
  }
}

/*
 * Application entry point.
 */
int main(void) {

  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  halInit();
  chSysInit();


  /*
   * Creates the blinker thread.
   */
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);
  chThdCreateStatic(waThread2, sizeof(waThread2), NORMALPRIO+1, Thread2, NULL);
  /*
   * Normal main() thread activity, in this demo it does nothing except
   * sleeping in a loop and check the button state.
   */

}