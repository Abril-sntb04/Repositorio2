#include "chconf.h"
#include "halconf.h"
#include "rt_test_root.h"
#include "oslib_test_root.h"
#include <chprintf.h>
#include <string.h>

#define BUFFER_SIZE 4

static objects_fifo_t miFifo;
static uint64_t fifoBuffer[BUFFER_SIZE];
static msg_t fifoMailbox[BUFFER_SIZE];

uint64_t num=12345;

static THD_WORKING_AREA(fifoReceive_thd_wa, 1024);
static THD_FUNCTION(fifoReceive_thd, arg) {

  (void)arg;
  chRegSetThreadName("fifoReceive");
  sdStart(&LPSD1, NULL);

  while(true) {
  uint64_t valorRecibido=0;
  uint64_t *pointerRecibeValor=&valorRecibido;
  uint64_t **pointerDelPointer=&pointerRecibeValor;

 chprintf(&LPSD1, "Pointer antes: %p \r\n", pointerRecibeValor);
  chprintf(&LPSD1, "Pointer del pointer antes: %p \r\n", pointerDelPointer);

  if(chFifoReceiveObjectTimeout(&miFifo, (void **)pointerDelPointer, TIME_INFINITE)==MSG_OK)
  {

      
    chprintf(&LPSD1, "Valor recibido 1: %d \r\n", valorRecibido);
    chprintf(&LPSD1, "Valor recibido 2: %d \r\n", (*pointerRecibeValor));

    chprintf(&LPSD1, "Pointer del valor recibido: %p \r\n", pointerRecibeValor);
    chprintf(&LPSD1, "Pointer del pointer del valor recibido: %p \r\n", pointerDelPointer);
    chFifoReturnObject(&miFifo, (void *)pointerDelPointer);
    }
   /*else
    {
    chprintf(&LPSD1, "Nada\r\n");
    }
    */ 
    
  }
}

int main(void) {

  halInit();
  chSysInit();

  chFifoObjectInit(&miFifo,sizeof(uint64_t),BUFFER_SIZE, (void *)fifoBuffer, fifoMailbox);
  chThdCreateStatic(fifoReceive_thd_wa, sizeof(fifoReceive_thd_wa), NORMALPRIO+3, fifoReceive_thd, NULL);
  
  palSetLineMode(LINE_LED_GREEN,PAL_MODE_OUTPUT_PUSHPULL);
/*
  chprintf(&LPSD1, "Pointer fifoMailbox[0]%p \r\n", fifoMailbox[0]));
  chprintf(&LPSD1, "Pointer fifoMailbox[1]%p \r\n", fifoMailbox[1]);
  chprintf(&LPSD1, "Pointer fifoMailbox[2]%p \r\n", fifoMailbox[2]);
  chprintf(&LPSD1, "Pointer fifoMailbox[3]%p \r\n", fifoMailbox[3]);
  */


  while(true)
  {
    uint64_t *receivedAddress= (uint64_t *)chFifoTakeObjectTimeout(&miFifo, TIME_IMMEDIATE); //Al pointer que me devuelve Take le estoy diciendo que es un pointer a un int de 64bits (8 bytes)
   (*receivedAddress)=num; //copiamos el valor de num a donde apunta el puntero
   //chprintf(&LPSD1, "Pointer que devuelve chTake: %p \r\n", receivedAddress);
   //chprintf(&LPSD1, "Numero al que apunta el pointer que devuelve chTake: %d \r\n", *receivedAddress);
   //chprintf(&LPSD1, "num: %d \r\n", num);
    chFifoSendObject(&miFifo, receivedAddress);

    num++;
    
    palToggleLine(LINE_LED_GREEN);
    chThdSleepMilliseconds(2000);
    
  }
}