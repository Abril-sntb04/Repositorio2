#include "chconf.h"
#include "halconf.h"
#include "rt_test_root.h"
#include "oslib_test_root.h"
#include <chprintf.h>
#include <string.h>


//LED blink timer 3 callback
static void timer3_led_cb(GPTDriver *gptp, void *arg) {

palToggleLine(LINE_LED_GREEN); //LED_toggle

}


static GPTConfig gpt3cfg = {
  10000,           // Frecuencia en Hz del timer
  timer3_led_cb   // Timer callback
};
 

int main(void) {

  halInit();
  chSysInit();
  
  //Configuración del pin del led como salida, y del pin del boton como entrada
  palSetLineMode(LINE_LED_GREEN,PAL_MODE_OUTPUT_PUSHPULL);
  palSetLineMode(LINE_USER_BUTTON,PAL_MODE_INPUT);

  //Habilitamos el evento de flanco ascendente en el pin del botón                    
  palEnableLineEvent(LINE_USER_BUTTON, PAL_EVENT_MODE_RISING_EDGE);

  sdStart(&LPSD1, NULL);
  
  gptStart(&GPTD3, &gpt3cfg); //Configurar y empezar timer 3
  gptStartContinuous(&GPTD3, 10000); //El timer expira cada 10000tics= 10000 x 1/100000 s = 1 segundo

  int n=0;

  while(true)
  {
    palWaitLineTimeout(LINE_USER_BUTTON, TIME_INFINITE);
    chprintf(&LPSD1, "Valor recibido: %d \r\n", n);
    n++;

    //Para evitar el debouncing que se pueda generar al presionar una vez el botón
    chThdSleepMilliseconds(50);
    
  }
}