#ifndef BLUETOOTH_H
	#define BLUETOOTH_H
	/*==================[inlcusiones]============================================*/

	#include <stdint.h>
	#include <stdlib.h>



	/*==================[definiciones y macros]==================================*/

	#define UART_PC        UART_USB
	#define UART_BLUETOOTH UART_232

	// Definiciones para la interrupcion bloqueante
	bool_t recibido;

	// Definiciones para la interrupcion no bloqueante
	uint8_t recibo;

	// Valores que recibo por bluetooth como cadena de caracteres
	char mensaje_recibido[20];
	char hora_recibida[5];
	char parametros_recibidos[20];					// tMax,tMin,hMax,hMin,hsInicio,hsFin

	void hc06Main( uint8_t data, float *, int horas, int minutos, int encendida, char [], char []);
	void hc06Config();
	bool hc05Test( int32_t uart );
	void hc05PrintATCommands( int32_t uart );
	void bluetoothInterruptRead ();

#endif
