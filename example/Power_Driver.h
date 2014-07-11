#ifndef POWER_DRIVER
#define POWER_DRIVER

#include "stm32l1xx_pwr.h"
#include "stm32l1xx_rcc.h"

#define QUEUE_SIZE 8

void enterLPMode();
void exitLPMode();


#endif
