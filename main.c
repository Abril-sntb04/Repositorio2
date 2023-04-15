

#include "chconf.h"
#include "halconf.h"
#include "rt_test_root.h"
#include "oslib_test_root.h"
#include <string.h>

static virtual_timer_t led_vt;


//LED timer callback
static void led_cb(void *arg) {

  palToggleLine(LINE_LED_GREEN); //LED_toggle
  chSysLockFromISR();
  chVTSetI(&led_vt, TIME_MS2I(500), led_cb, NULL);
  chSysUnlockFromISR();
  
}

static THD_WORKING_AREA(waThread2, 1024);
static THD_FUNCTION(Thread2, arg) {

  (void)arg;
  chRegSetThreadName("PuertoSerie");
  /*
   * Activates the serial driver 2 using the driver default configuration.
   */

  sdStart(&LPSD1, NULL);

  while(true) {
    sdWrite(&LPSD1,"Hello world\r\n", strlen("Hello world\r\n"));
    chThdSleepMilliseconds(500);
  }
}



int main(void) {

  halInit();
  chSysInit();

  /* LED timer initialization.*/
  chVTObjectInit(&led_vt);
  chThdCreateStatic(waThread2, sizeof(waThread2), NORMALPRIO+1, Thread2, NULL);

  chVTSet(&led_vt, TIME_MS2I(100), led_cb, NULL);


  return 0;
}