/* ###################################################################
**     Filename    : main.c
**     Project     : xadow_phone_gps_v2
**     Processor   : MKL02Z32VFG4
**     Version     : Driver 01.01
**     Compiler    : GNU C Compiler
**     Date/Time   : 2015-07-29, 10:10, # CodeGen: 0
**     Abstract    :
**         Main module.
**         This module contains user's application code.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/*!
** @file main.c
** @version 01.01
** @brief
**         Main module.
**         This module contains user's application code.
*/         
/*!
**  @addtogroup main_module main module documentation
**  @{
*/         
/* MODULE main */


/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "Events.h"
#include "clockMan1.h"
#include "pin_mux.h"
#include "osa1.h"
#include "delay.h"
#include "i2cCom1.h"
#include "gpio.h"
#include "uart.h"
#include "i2c.h"
#include "gps.h"


unsigned char sys_gpio_test = 0;


void GPIO_Test_for_TE(void);


int main(void)
{
	PE_low_level_init();
	gpio_init();
	uart_init();
	i2c_init();
	gps_sate_data_init();

	while(1)
	{
		// Wait to receive input data
		if(kStatus_LPSCI_Success == LPSCI_HAL_ReceiveDataPolling(UART0, &SBUF,1))
		{
			// Echo received character
			// LPSCI_HAL_SendDataPolling(UART0, &SBUF, 1);
			gps_parser();
		}
		GPIO_Test_for_TE();
	}
}

void GPIO_Test_for_TE(void)
{
	if(sys_gpio_test)
	{
		sys_gpio_test = 0;

		GPIO_HAL_SetPinDir(PTA, 4, kGpioDigitalOutput); // INT1 pin
		GPIO_HAL_SetPinDir(PTB, 5, kGpioDigitalInput); // INT2 pin

		PORT_HAL_SetMuxMode(PORTB,3u,kPortMuxAsGpio);
		PORT_HAL_SetMuxMode(PORTB,4u,kPortMuxAsGpio);
		GPIO_HAL_SetPinDir(PTB, 3, kGpioDigitalOutput); // SCL pin
		GPIO_HAL_SetPinDir(PTB, 4, kGpioDigitalOutput); // SDA pin

		while(1)
		{
			if(GPIO_HAL_ReadPinInput(PTB, 5))
			{
				GPIO_HAL_ClearPinOutput(PTA, 4); // INT1 set low
				GPIO_HAL_SetPinOutput(PTB, 3); // SCL set high
				GPIO_HAL_ClearPinOutput(PTB, 4); // SDA set low
			}
			else
			{
				GPIO_HAL_SetPinOutput(PTA, 4); // INT1 set high
				GPIO_HAL_ClearPinOutput(PTB, 3); // SCL set low
				GPIO_HAL_SetPinOutput(PTB, 4); // SDA set high
			}

			delay_ms(100);
		}
	}
}
