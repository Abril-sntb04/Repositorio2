

#include "chconf.h"
#include "halconf.h"
#include "rt_test_root.h"
#include "oslib_test_root.h"
#include <string.h>

static virtual_timer_t led_vt;


//LED timer callback
static void led_cb(void *arg) {

  palToggleLine(LINE_LED_GREEN); //LED_toggle
  chVTSet(&led_vt, TIME_MS2I(100), led_cb, NULL);
  
}

int main(void) {

  halInit();
  chSysInit();

  /* LED timer initialization.*/
  chVTObjectInit(&led_vt);


  chVTSet(&led_vt, TIME_MS2I(100), led_cb, NULL);

  return 0;
}