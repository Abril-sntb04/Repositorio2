#include "chconf.h"
#include "halconf.h"
#include "rt_test_root.h"
#include "oslib_test_root.h"
#include <chprintf.h>
#include <string.h>

#define BUFFER_SIZE 4

static objects_fifo_t miFifo;
static uint64_t fifoBuffer[BUFFER_SIZE];
static uint64_t fifoMailbox[BUFFER_SIZE];

uint64_t num=12345;

static THD_WORKING_AREA(fifoReceive_thd_wa, 1024);
static THD_FUNCTION(fifoReceive_thd, arg) {

  (void)arg;
  chRegSetThreadName("fifoReceive");
  sdStart(&LPSD1, NULL);

  while(true) {

  uint64_t *pointerRecibeValor;
  uint64_t *pointerDelPointer=&pointerRecibeValor;

    if(chFifoReceiveObjectTimeout(&miFifo, pointerDelPointer, TIME_INFINITE)==MSG_OK)
    {

      uint64_t valorRecibido= (*pointerRecibeValor);
      
      chprintf(&LPSD1, "Valor recibido: %d \r\n", valorRecibido);
     // chprintf(&LPSD1, "Pointer del valor recibido: %p \r\n", pointerRecibeValor);
     // chprintf(&LPSD1, "Pointer del pointer del valor recibido: %p \r\n", pointerDelPointer);
      chFifoReturnObject(&miFifo, pointerDelPointer);
    }
    else
    {
    chprintf(&LPSD1, "Nada\r\n");
    }
    
  }
}

int main(void) {

  halInit();
  chSysInit();


  chFifoObjectInit(&miFifo,sizeof(fifoBuffer),BUFFER_SIZE, (void *)fifoBuffer, fifoMailbox);
  chThdCreateStatic(fifoReceive_thd_wa, sizeof(fifoReceive_thd_wa), NORMALPRIO+3, fifoReceive_thd, NULL);
  
  palSetLineMode(LINE_LED_GREEN,PAL_MODE_OUTPUT_PUSHPULL);

  while(true)
  {
    uint64_t *receivedAddress= (uint64_t *)chFifoTakeObjectTimeout(&miFifo, TIME_IMMEDIATE); //Al pointer que me devuelve Take le estoy diciendo que es un pointer a un int de 64bits (8 bytes)
   (*receivedAddress)=num; //Incrementamos el valor al que hace referencia receivedAdress (nuestro numero que queremos incrementar)
    chFifoSendObject(&miFifo, receivedAddress);

    num++; //Incrementamos el valor al que hace referencia receivedAdress (nuestro numero que queremos incrementar)
    
    palToggleLine(LINE_LED_GREEN);
    chThdSleepMilliseconds(200);
    
  }
}