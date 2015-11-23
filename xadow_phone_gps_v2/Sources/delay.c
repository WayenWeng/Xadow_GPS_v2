/*
 * delay.c
 *
 *  Created on: 2015Äê8ÔÂ10ÈÕ
 *      Author: Wayen
 */

#include <stdint.h>
#include "fsl_os_abstraction.h"
#include "delay.h"

void delay_init(void)
{
	OSA_Init();
}

void delay_ms(uint32_t time)
{
	OSA_TimeDelay(time);
}
