/*
 * delay.c
 *
 *  Created on: 2015��8��10��
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
