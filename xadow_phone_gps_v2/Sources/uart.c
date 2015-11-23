
#include <stdint.h>
#include "board.h"
#include "uart.h"
#include "clockMan1.h"
#include "fsl_lpsci_hal.h"


uint8_t SBUF = 0;


void uart_init(void)
{
	uint32_t lpsciSourceClock = 0;

	// Set the LPSCI clock source selection
	CLOCK_SYS_SetLpsciSrc(0, kClockLpsciSrcFll);
	// Enable the clock for LPSCI module
	CLOCK_SYS_EnableLpsciClock(0);
	// Gets the clock frequency for LPSCI module
	lpsciSourceClock = CLOCK_SYS_GetLpsciFreq(0);
	// Initialize LPSCI baud rate, bit count, parity and stop bit
	LPSCI_HAL_SetBaudRate(UART0, lpsciSourceClock, 9600);
	LPSCI_HAL_SetBitCountPerChar(UART0, kLpsci8BitsPerChar);
	LPSCI_HAL_SetParityMode(UART0, kLpsciParityDisabled);
	LPSCI_HAL_SetStopBitCount(UART0, kLpsciOneStopBit);
	// Enable the LPSCI transmitter and receiver
	LPSCI_HAL_EnableTransmitter(UART0);
	LPSCI_HAL_EnableReceiver(UART0);
}
