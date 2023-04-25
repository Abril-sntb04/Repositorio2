

#include "chconf.h"
#include "halconf.h"
#include "rt_test_root.h"
#include "oslib_test_root.h"
#include <chprintf.h>
#include <string.h>

static volatile msg_t mensaje=200;
static msg_t msg_buffer[4];
static mailbox_t mailbox_object;



//LED blink timer 3 callback
static void timer3_led_cb(GPTDriver *gptp, void *arg) {

palToggleLine(LINE_LED_GREEN); //LED_toggle
  
}


static GPTConfig gpt3cfg = {
  10000,           // Frecuencia en Hz del timer
  timer3_led_cb   // Timer callback
};



//Cuando se presiona el boton se llama a esta función 
static void boton_cb(void *arg) {
  (void)arg;

  chSysLockFromISR();
  chMBPostI(&mailbox_object, mensaje);
  chSysUnlockFromISR();
  mensaje++;

}

static THD_WORKING_AREA(serial_thd_wa, 1024);
static THD_FUNCTION(serial_thd, arg) {

  (void)arg;
  chRegSetThreadName("ImprimirPuerto");
  sdStart(&LPSD1, NULL);
  chMBObjectInit(&mailbox_object, &msg_buffer, sizeof(msg_buffer));

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
  
  //Configuración del pin del led como salida, y del pin del boton como entrada
  palSetLineMode(LINE_LED_GREEN,PAL_MODE_OUTPUT_PUSHPULL);
  palSetLineMode(LINE_USER_BUTTON,PAL_MODE_INPUT);

  //Habilitamos el evento de flanco ascendente en el pin del botón                    
  palEnableLineEvent(LINE_USER_BUTTON, PAL_EVENT_MODE_RISING_EDGE);

  //Configuramos la función callback cuando ocurra un flanco ascendente en el pin del botón
  palSetLineCallback(LINE_USER_BUTTON, boton_cb, NULL);


  gptStart(&GPTD3, &gpt3cfg); //Configurar y empezar timer 3
  gptStartContinuous(&GPTD3, 10000); //El timer expira cada 10000tics= 10000 x 1/100000 s = 1 segundo

 
  return 0;
}