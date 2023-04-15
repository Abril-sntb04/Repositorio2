

#include "chconf.h"
#include "halconf.h"
#include "rt_test_root.h"
#include "oslib_test_root.h"
#include <string.h>

static virtual_timer_t led_vt;
static virtual_timer_t serial_vt;


//LED timer callback
static void led_cb(void *arg) {

  palToggleLine(LINE_LED_GREEN); //LED_toggle
  chSysLockFromISR();
  chVTSetI(&led_vt, TIME_MS2I(500), led_cb, NULL);
  chSysUnlockFromISR();
  
}

//Serial Port timer2 callback
static void serial_cb(void *arg) {

    sdWrite(&LPSD1,"Hello world\r\n", strlen("Hello world\r\n"));
    chSysLockFromISR();
    chVTSetI(&serial_vt, TIME_MS2I(1000), serial_cb, NULL);
    chSysUnlockFromISR();
  
}


int main(void) {

  halInit();
  chSysInit();

  sdStart(&LPSD1, NULL);

  /* LED timer initialization.*/
  chVTObjectInit(&led_vt);
  chVTObjectInit(&serial_vt);


  chVTSet(&led_vt, TIME_MS2I(500), led_cb, NULL);
  chVTSet(&serial_vt, TIME_MS2I(1000), serial_cb, NULL);


  return 0;
}