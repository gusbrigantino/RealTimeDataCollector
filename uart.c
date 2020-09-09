//
// COMP/GENG 422 - Gus Brigantino
//
// UART module
//

#include "main.h"
#include "uart.h"


extern UART_HandleTypeDef	hDiscoUart;		// Discover UART handle


void UartInit(void)
{
	// Initialize the UART to 115.2Kbps, 8 data bits, 1 stop bit, no parity
	hDiscoUart.Instance = DISCOVERY_COM1;
	hDiscoUart.Init.BaudRate = 115200;
	hDiscoUart.Init.WordLength = UART_WORDLENGTH_8B;
	hDiscoUart.Init.StopBits = UART_STOPBITS_1;
	hDiscoUart.Init.Parity = UART_PARITY_NONE;
	hDiscoUart.Init.Mode = UART_MODE_TX_RX;
	hDiscoUart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	hDiscoUart.Init.OverSampling = UART_OVERSAMPLING_16;
	hDiscoUart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	hDiscoUart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	BSP_COM_Init(COM1, &hDiscoUart);
}


int UartCharWrite(int Char)
{
	// Write a character to the serial port and wait until it has been sent
	while (HAL_UART_Transmit(&hDiscoUart, (uint8_t *) &Char, 1, 30000) != HAL_OK)
		;

	return Char;
}
