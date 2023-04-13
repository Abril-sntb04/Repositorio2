#!/bin/bash
make -j8
openocd -f openocd/stlink.cfg -f openocd/stm32g4x.cfg -c "program build/ch.elf verify reset exit"make 