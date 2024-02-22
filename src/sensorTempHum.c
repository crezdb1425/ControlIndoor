#include "sensorTempHum.h"

// FUNCION PRINCIPAL
void dht11Main( float * sensado)
{
   // Variables para almacenar humedad y temperatura
	float humidity = 0, temperature = 0;

	// Lectura del sensor DHT11, devuelve true si pudo leer correctamente
	if( dht11Read(&humidity, &temperature) ) {
		 sensado[0] = humidity;
		 sensado[1] = temperature;
	} else {
		 // Informo el error de lectura
		 printf( "Error al leer DHT11.\r\n\r\n");
	}
}
