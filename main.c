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
  uint64_t num_recibido;

    if(chFifoReceiveObjectTimeout(&miFifo,&num_recibido, TIME_INFINITE)==MSG_OK)
    {
      chprintf(&LPSD1, "Valor recibido: %d \r\n",num_recibido);
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
   
    chFifoTakeObjectTimeout(&miFifo, TIME_IMMEDIATE);
    chFifoSendObject(&miFifo, num);
    num++;
    
    palToggleLine(LINE_LED_GREEN);
    chThdSleepMilliseconds(200);
    
  }
}