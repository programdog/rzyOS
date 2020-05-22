#ifndef RZYOSARCH_H
#define RZYOSARCH_H


#if defined(STM32F40_41xxx) || defined(STM32F40XX)
	#include "stm32f407xx.h"
#endif

#ifdef ARMCM3_SIM
	#include "ARMCM3.h"
#endif


#endif
