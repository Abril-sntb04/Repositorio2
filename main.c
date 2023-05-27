#include "chconf.h"
#include "halconf.h"
#include "rt_test_root.h"
#include "oslib_test_root.h"
#include <chprintf.h>
#include <string.h>

#define BUFFER_SIZE 4

struct myStruct
{
  char text[15];
  int id;
  bool b;
};

static objects_fifo_t miFifo;
static struct myStruct fifoBuffer[BUFFER_SIZE];
static msg_t fifoMailbox[BUFFER_SIZE];

struct myStruct s1;
int num=2222;

static THD_WORKING_AREA(fifoReceive_thd_wa, 1024);
static THD_FUNCTION(fifoReceive_thd, arg) {

  (void)arg;
  chRegSetThreadName("fifoReceive");
 

  while(true) {
  //uint64_t valorRecibido=0;
  struct myStruct *pointerObjeto;

  if(chFifoReceiveObjectTimeout(&miFifo, (void **)&pointerObjeto, TIME_MS2I(1000))==MSG_OK)
  {

    chprintf(&LPSD1, "%s %d %d \r\n\r\n", pointerObjeto->text, pointerObjeto->id, pointerObjeto->b);

    //chprintf(&LPSD1, "& a la que apunta pointer %p \r\n\r\n", pointerObjeto);

    chFifoReturnObject(&miFifo, (void *)pointerObjeto);
    }
    
  }
}

int main(void) {

  halInit();
  chSysInit();

  chFifoObjectInit(&miFifo,sizeof(struct myStruct),BUFFER_SIZE, (void *)&fifoBuffer, &fifoMailbox[0]);
  
  sdStart(&LPSD1, NULL);

  chThdCreateStatic(fifoReceive_thd_wa, sizeof(fifoReceive_thd_wa), NORMALPRIO+3, fifoReceive_thd, NULL);
  
  palSetLineMode(LINE_LED_GREEN,PAL_MODE_OUTPUT_PUSHPULL);
 


  while(true)
  {

    struct myStruct *receivedAddress= chFifoTakeObjectTimeout(&miFifo, TIME_IMMEDIATE); //Al pointer que me devuelve Take le estoy diciendo que es un pointer a un int de 64bits (8 bytes)
   //copiamos el valor de num a donde apunta el puntero
   
   if(receivedAddress!=NULL)
   {
    //chprintf(&LPSD1, "& que devuelve chTake: %p \r\n", receivedAddress);
    strcpy(receivedAddress->text, "Hello world"); 
    receivedAddress->id=num; 
    receivedAddress->b=true; 
    chFifoSendObject(&miFifo, receivedAddress);
    num++;
   }
   
    palToggleLine(LINE_LED_GREEN);
    chThdSleepMilliseconds(2000);
    
  }
}