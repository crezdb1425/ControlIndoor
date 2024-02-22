#include "sapi.h"
#include <string.h>
#include <stdio.h>
#include "display.h"


// Definir caracteres personalizados: https://omerk.github.io/lcdchargen/


//Temperatura - Termometro
const char tempChar[8] = {
   0b01110,
   0b01010,
   0b01010,
   0b01110,
   0b01110,
   0b10111,
   0b11111,
   0b01110
};

// Humedad - Gota
const char humChar[8] = {
   0b00100,
   0b00100,
   0b01110,
   0b10111,
   0b10111,
   0b10011,
   0b01110,
   0b00000
};


// Simbolo grados
const char graChar[8] = {
   0b01110,
   0b01010,
   0b01110,
   0b00000,
   0b00000,
   0b00000,
   0b00000,
   0b00000
};

// Simbolo luz
const char luzChar[8] = {
	0b01010,
	0b01010,
	0b00000,
	0b01110,
	0b01110,
	0b10001,
	0b11011,
	0b11011
};

char tempString[4];
char humString[4];
char luzHString[3];
char luzMString[3];
char t;
char h;


// FUNCION PRINCIPAL
void displayMain( float * sensado, struct LUZS luz){
	// Convierto los valores que recibo a string
	gcvt (sensado[0], 2, humString);
	gcvt (sensado[1], 2, tempString);
	sprintf(luzHString, "%d", luz.horas);
	sprintf(luzMString, "%d", luz.minutos);

	lcdClear();                     // Borrar la pantalla

   	lcdGoToXY( 0, 0 ); // Poner cursor en 0, 0
	lcdSendStringRaw( "Tmp" );
	lcdData(0);
	lcdSendStringRaw( " Hum" );
	lcdData(1);
	if(luz.dia)
		lcdSendStringRaw( " Luz ON" );
	else
		lcdSendStringRaw( " LuzOFF" );
	lcdData(2);

	lcdGoToXY( 0, 1 );
	lcdSendStringRaw( tempString );
	lcdData(7);
	lcdSendStringRaw( "C" );

	lcdGoToXY( 5, 1 );
	lcdSendStringRaw( humString );
	lcdSendStringRaw( "%" );

	lcdGoToXY( 9, 1 );
	lcdSendStringRaw( luzHString );
	lcdSendStringRaw( ":" );
	lcdSendStringRaw( luzMString );
	lcdSendStringRaw( "hs" );


	//lcdClear(); // Borrar la pantalla
	//lcdGoToXY( 0, 0 );
}

/*==================[definiciones de funciones internas]=====================*/

void displayInit(){
	// Inicializar LCD de 16x2 (caracteres x lineas) con cada caracter de 5x2 pixeles
   lcdInit( 16, 2, 5, 8 );

   // Cargar el caracter a CGRAM
   // El primer parametro es el codigo del caracter (0 a 7).
   // El segundo es el puntero donde se guarda el bitmap (el array declarado
   // anteriormente)
   lcdCreateChar( 0, tempChar );
   lcdCreateChar( 1, humChar );
   lcdCreateChar( 2, luzChar );

   lcdCreateChar( 7, graChar );

   lcdCursorSet( LCD_CURSOR_OFF ); // Apaga el cursor
}

/*==================[definiciones de funciones externas]=====================*/

/*==================[fin del archivo]========================================*/
