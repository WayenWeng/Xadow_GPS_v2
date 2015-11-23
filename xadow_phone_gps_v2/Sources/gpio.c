
#include "gpio.h"
#include "fsl_gpio_hal.h"


void gpio_init(void)
{
	GPIO_HAL_SetPinDir(PTA, 3, kGpioDigitalInput); // 32K pin
	GPIO_HAL_SetPinDir(PTA, 4, kGpioDigitalOutput); // INT1 pin
	GPIO_HAL_SetPinDir(PTA, 5, kGpioDigitalOutput); // RESET_N pin
	GPIO_HAL_SetPinDir(PTA, 6, kGpioDigitalOutput); // FORCE_ON pin
	GPIO_HAL_SetPinDir(PTA, 7, kGpioDigitalInput); // PPS pin
	GPIO_HAL_SetPinDir(PTB, 0, kGpioDigitalInput); // UI_FIX pin
	GPIO_HAL_SetPinDir(PTB, 5, kGpioDigitalInput); // INT2 pin
	GPIO_HAL_SetPinOutput(PTA, 4); // INT1 set high
	GPIO_HAL_SetPinOutput(PTA, 5); // RESET_N pin set high
	GPIO_HAL_SetPinOutput(PTA, 6); // FORCE_ON pin set high
}



