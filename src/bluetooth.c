

/*==================[definiciones y macros]==================================*/

#include "sapi.h"
#include <string.h>
#include "bluetooth.h"


char h [4];
char t [4];
char hs [3];
char min [3];
char enc [2];
char datos[10];


/**************** DECLARACION DE FUNCIONES *****************/



/**************** DEFINICION DE FUNCIONES ******************/

void hc06Main(uint8_t data, float * sensado, int horas, int minutos, int encendida, char estados_actuadores[4], char nivel_agua[3])
{
	// Convierto a string y concateno mensaje final para enviar separado por coma
	  gcvt (sensado[1], 2, t);
	  gcvt (sensado[0], 2, h);
	  itoa (horas, hs, 10);
	  itoa (minutos, min, 10);
	  itoa (encendida, enc, 10);
	  sprintf(datos, "%s,%s,%s:%s,%s,%c,%c,%c,%c,%s", t, h, hs, min, enc, estados_actuadores[0], estados_actuadores[1], estados_actuadores[2], estados_actuadores[3], nivel_agua);
	  printf("Al BT: %s, %s, %s:%s, %s, %c, %c, %c, %c ,%s \r\n", t, h, hs, min, enc, estados_actuadores[0], estados_actuadores[1], estados_actuadores[2], estados_actuadores[3], nivel_agua);
	  uartWriteString( UART_BLUETOOTH, datos );
}

void bluetoothInterruptRead (void *noUsado) {
	//uartReadByte( UART_BLUETOOTH, &datos );		// Recibo los datos cargados en la app al presionar "ACTUALIZAR"
	if (uartReadByte( UART_BLUETOOTH, &recibo )){
		uartWriteString( UART_BLUETOOTH, "HOLAAAAAAAAAA\r\n" );
		uartWriteString( UART_USB, recibo );
		strcat(mensaje_recibido,recibo);
		if(mensaje_recibido[0] == 'h' || mensaje_recibido[0] == 'p'){
			if(strlen(mensaje_recibido) == 5){
				strcpy(hora_recibida,mensaje_recibido);
				uartWriteString( UART_BLUETOOTH, "recibi la hora\r\n" );
				uartWriteString( UART_USB, "recibi la hora\r\n" );
				uartWriteString( UART_USB, mensaje_recibido );
				uartWriteString( UART_BLUETOOTH, mensaje_recibido );
			}
			else {
				if(strlen(mensaje_recibido) > 5){
					strcpy(parametros_recibidos,mensaje_recibido);
					uartWriteString( UART_BLUETOOTH, "recibi los parametros\r\n" );
				}
			}
		}
		}
}

void hc06Config()
{
	   // Inicializar UART_USB para conectar a la PC
	   //uartConfig( UART_PC, 9600 );
	   //uartWriteString( UART_PC, "UART_PC configurada.\r\n" );

	   // Inicializar UART_232 para conectar al modulo bluetooth
	   uartConfig( UART_BLUETOOTH, 9600 );
	   uartWriteString( UART_PC, "UART_BLUETOOTH para modulo Bluetooth configurada.\r\n" );

	   uartWriteString( UART_PC, "Testeto si el modulo esta conectado enviando: AT\r\n" );
	   if( hc05Test( UART_BLUETOOTH ) ){
	      uartWriteString( UART_PC, "Modulo conectado correctamente.\r\n" );
	   }
	   else{
	      uartWriteString( UART_PC, "No funciona.\r\n" );
	   }
}

bool hc05Test( int32_t uart )
{
   uartWriteString( uart, "AT\r\n" );
   return waitForReceiveStringOrTimeoutBlocking( uart,
                                                 "OK\r\n", strlen("OK\r\n"),
                                                 1000 );
}
