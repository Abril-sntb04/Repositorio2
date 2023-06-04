#include "chconf.h"
#include "halconf.h"
#include "rt_test_root.h"
#include "oslib_test_root.h"
#include <chprintf.h>
#include <string.h>

#define PIPE_SIZE 16

static uint8_t buffer[PIPE_SIZE];
PIPE_DECL(pipe1, buffer, PIPE_SIZE);


static THD_WORKING_AREA(serialDebug_thd_wa, 1024);
static THD_FUNCTION(serialDebug_thd, arg) {

  (void)arg;
  chRegSetThreadName("serialDebug");


  while(true) {
    
    uint8_t buf[PIPE_SIZE];

      //size_t chPipeReadTimeout(pipe_t *pp, uint8_t *bp, size_t n, sysinterval_t timeout);

      size_t bytesRead = chPipeReadTimeout(&pipe1, buf, PIPE_SIZE, TIME_IMMEDIATE);
      chprintf(&LPSD1, "Bytes read: %d\r\n", bytesRead);
      chprintf(&LPSD1, "Message: %s\r\n", buf);
       chThdSleepMilliseconds(500);

  }
}



int main(void) {

  halInit();
  chSysInit();

  sdStart(&LPSD1, NULL);
  //chPipeObjectInit(&pipe1, buffer, PIPE_SIZE);
  chThdCreateStatic(serialDebug_thd_wa, sizeof(serialDebug_thd_wa), NORMALPRIO, serialDebug_thd, NULL);
  

  palSetLineMode(LINE_LED_GREEN,PAL_MODE_OUTPUT_PUSHPULL);

  while(true)
  {
    size_t bytesWritten = chPipeWriteTimeout(&pipe1, "Hello", PIPE_SIZE, TIME_IMMEDIATE);

    //size_t chPipeWriteTimeout(pipe_t *pp, const uint8_t *bp, size_t n, sysinterval_t timeout);

   chprintf(&LPSD1, "Bytes written: %d\r\n", bytesWritten);
    
    palToggleLine(LINE_LED_GREEN);
    chThdSleepMilliseconds(500);
    
  }
}