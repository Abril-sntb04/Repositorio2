

#include "chconf.h"
#include "halconf.h"
#include "rt_test_root.h"
#include "oslib_test_root.h"
#include <string.h>

//static virtual_timer_t led_vt;
static virtual_timer_t serial_vt;

static thread_reference_t trp = NULL;

//LED timer callback
/*
static void led_cb(void *arg) {

  palToggleLine(LINE_LED_GREEN); //LED_toggle
  chSysLockFromISR();
  chVTSetI(&led_vt, TIME_MS2I(100), led_cb, NULL);
  chSysUnlockFromISR();
  
}
*/

//Serial Port timer2 callback
static void serial_cb(void *arg) {

    chSysLockFromISR();
    chThdResumeI(&trp, (msg_t)0x1337); 
    chSysUnlockFromISR();
  
}

static THD_WORKING_AREA(waThread, 1024);
static THD_FUNCTION(Thread, arg) {

  (void)arg;
  chRegSetThreadName("ImprimirPuerto");
  sdStart(&LPSD1, NULL);

  while(true) {
  msg_t msg;
    /*
    Bloqueamos el funcionamiento del nucleo en espera 
    a que el timer virtual termine
    */
    chSysLock();
    msg = chThdSuspendS(&trp);
    chSysUnlock();

    //Desde el callback del timer se reanuda y sigue con sus operaciones
    
    sdWrite(&LPSD1,"Hello world\r\n", strlen("Hello world\r\n"));

    //Seteamos el timer de vuelta para que siga imprimiendo cada 100ms
    chSysLockFromISR();
    chVTSetI(&serial_vt, TIME_MS2I(1000), serial_cb, NULL);
    chSysUnlockFromISR();

  }
}

int main(void) {

  halInit();
  chSysInit();

  /* Timers initialization.*/
  //chVTObjectInit(&led_vt);
  chVTObjectInit(&serial_vt);

  chThdCreateStatic(waThread, sizeof(waThread), NORMALPRIO, Thread, NULL);

  chVTSet(&serial_vt, TIME_MS2I(1000), serial_cb, NULL);
  //chVTSet(&led_vt, TIME_MS2I(100), led_cb, NULL);

  
  return 0;
}