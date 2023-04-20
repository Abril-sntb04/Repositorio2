

#include "chconf.h"
#include "halconf.h"
#include "rt_test_root.h"
#include "oslib_test_root.h"
#include <chprintf.h>
#include <string.h>

static virtual_timer_t led_vt;
static virtual_timer_t serial_vt;


static volatile msg_t mensaje=200;
static msg_t msg_buffer[1];
static mailbox_t mailbox_object;


//LED timer callback
static void led_cb(virtual_timer_t *vtp, void *arg) {
 
  chSysLockFromISR();
  palToggleLine(LINE_LED_GREEN); //LED_toggle
  chVTSetI(&led_vt, TIME_MS2I(100), led_cb, NULL);
  chSysUnlockFromISR();
  
}

//Serial Port timer2 callback
static void serial_cb(virtual_timer_t *vtp, void *arg) {

    chSysLockFromISR();
    chMBPostI(&mailbox_object, mensaje);
    
    //Seteamos el timer de vuelta para que siga imprimiendo cada 100ms
    chVTSetI(&serial_vt, TIME_MS2I(1000), serial_cb, NULL);
    chSysUnlockFromISR();
}

static THD_WORKING_AREA(serial_thd_wa, 1024);
static THD_FUNCTION(serial_thd, arg) {

  (void)arg;
  chRegSetThreadName("ImprimirPuerto");
  sdStart(&LPSD1, NULL);

  //Seteamos timer del puerto serial por primera vez
  chVTSet(&serial_vt, TIME_MS2I(1000), serial_cb, NULL);

  while(true) {
  msg_t mensaje_recibido;
  uint8_t valor_byte[4];
  char buffer[10];

    chMBFetchTimeout(&mailbox_object, &mensaje_recibido, TIME_INFINITE);
    chprintf(&LPSD1, ("Valor recibido: "));

    for(int i=0; i<4; i++)
    {

      valor_byte[i]= mensaje_recibido >> i*8;
      chprintf(&LPSD1, ("%d", valor_byte[i]));
      //chsnprintf(buffer, sizeof(valor_byte[i]), ("%d",valor_byte[i]));
    }
    //sdWrite(&LPSD1, ("Valor recibido: %s\r\n",buffer), strlen("Valor recibido: %s\r\n",buffer));
    chprintf(&LPSD1, "\r\n");
  }
}

int main(void) {

  halInit();
  chSysInit();

  /* Timers initialization.*/
  chVTObjectInit(&led_vt);
  chVTObjectInit(&serial_vt);

  chMBObjectInit(&mailbox_object, msg_buffer, sizeof(msg_buffer));

  //Thread puerto serial
  chThdCreateStatic(serial_thd_wa, sizeof(serial_thd_wa), NORMALPRIO+3, serial_thd, NULL);

  //Seteamos el timer del led por priemra vez
  chVTSet(&led_vt, TIME_MS2I(100), led_cb, NULL);
  
  return 0;
}