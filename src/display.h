#ifndef DISPLAY_H
#define DISPLAY_H


/*==================[inlcusiones]============================================*/

#include "sapi.h"        // <= Biblioteca sAPI
#include <stdint.h>

/*==================[definiciones y macros]==================================*/


struct LUZS {
	int horas;
	int minutos;
	int dia;			// dia = encendida = TRUE
};

/*==================[definiciones de datos internos]=========================*/

/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/

void displayInit();
void displayMain( float * sensado, struct LUZS luz);
void bluetoothInterruptRead();


// Definir caracteres personalizados: https://omerk.github.io/lcdchargen/


#endif
