

#include "chconf.h"
#include "halconf.h"
#include "rt_test_root.h"
#include "oslib_test_root.h"
#include <chprintf.h>
#include <string.h>
//#include <inttypes.h>

static virtual_timer_t serial_vt;

static volatile msg_t mensaje=1220;
static msg_t msg_buffer[4];
static mailbox_t mailbox_object;



//LED blink timer 3 callback
static void timer3_led_cb(GPTDriver *gptp, void *arg) {

  chSysLockFromISR();
  palToggleLine(LINE_LED_GREEN); //LED_toggle
  chSysUnlockFromISR();
  
}


static GPTConfig gpt3cfg = {
  1000,           // Timer clock ?
  timer3_led_cb   // Timer callback
};



//Serial Port timer2 callback
static void serial_cb(virtual_timer_t *vtp, void *arg) {

    chMBObjectInit(&mailbox_object, msg_buffer, sizeof(msg_buffer));
    mensaje++;
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
  chVTObjectInit(&serial_vt);
  sdStart(&LPSD1, NULL);

  //Seteamos timer del puerto serial por primera vez
  chVTSet(&serial_vt, TIME_MS2I(1000), serial_cb, NULL);

  while(true) {
  msg_t mensaje_recibido;

    chMBFetchTimeout(&mailbox_object, &mensaje_recibido, TIME_INFINITE);
    chprintf(&LPSD1, "Valor recibido: %d \r\n",mensaje_recibido);
  }
}

int main(void) {

  halInit();
  chSysInit();

  //Thread puerto serial
  chThdCreateStatic(serial_thd_wa, sizeof(serial_thd_wa), NORMALPRIO+3, serial_thd, NULL);


  palSetLineMode(LINE_LED_GREEN,PAL_MODE_OUTPUT_PUSHPULL);

  gptStart(&GPTD3, &gpt3cfg); //Configurar y empezar timer 3
  gptStartContinuous(&GPTD3, 1000); //El timer expira cada 1000ms = 1 segundo


/*
   palSetPadMode(IOPORT2, 7, PAL_MODE_OUTPUT_PUSHPULL);

  sdStart(&SD1, NULL);
  gptStart(&GPTD3, &gpt3cfg);

  gptStartContinuous(&GPTD3, 500);

  while (1) {
    chprintf(chp, "OCR3A: %d, TCCR3B: %x, period: %d, counter: %d , TCNT3: %d\r\n",
                   OCR3A,
                   TCCR3B,
                   GPTD3.period,
                   GPTD3.counter,
                   TCNT3);
    chThdSleepMilliseconds(100);
  }
}
  
  */
 
  return 0;
}