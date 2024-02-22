/*==================[inlcusiones]============================================*/

#include "sapi.h"        // <= Biblioteca sAPI
#include <string.h>
#include "bluetooth.h"
#include "display.h"
#include "sensorTempHum.h"

/*==================[definiciones y macros]==================================*/
void inicializacion(char hora[], char parametros[30]);
void actualizar_valores(char * parametros);
void hsConvert(char * h);
void estados_indoor();
void control_parametros();
void scheduleAndDispatchTasks();
void convertirMinutosAHoras(int);


const char LEDVERDE = GPIO1;
const char LEDROJO = GPIO3;
const char LEDAMARILLO = GPIO5;
const char SENSORHYT = GPIO2;
const char RELE1A = T_COL2;		// intractor
const char RELE1B = T_FIL1;		// calentador
const char RELE2A = ENET_RXD0;	// extractor
const char RELE2B = SPI_MOSI;	// luz

enum Valores{
	TAHA,
	TAHB,
	TAHC,
	TCHA,
	TCHB,
	TCHC
};

struct time{
	uint8_t hora;
	uint8_t minuto;
};


struct valores{
	char temp[3];
	char hum[3];
	char agua[3];
};

struct time hora_actual;
struct time hora_inicio;			// horas de inicio y fin determinadas por el usuario
struct time hora_fin;
struct LUZS luz_faltan;   			// horas de luz que faltan

float sensado [2];					// 0=humedad, 1_temp


uint8_t parametros_cargados[4];			// tMax, tMin, hMax, hMin
uint8_t parametros_actuales[4];			// tMax, tMin, hMax, hMin
uint8_t cotas_actuales[8];				// tMaxSup, tMaxInf, tMinSup, tMinInf, hMaxSup, hMaxInf, hMinSup, hMinInf
										//		0		1		2		 3		  4			5		6		7
struct valores val;					// Valores actuales - Temp, Hum, NivelAgua
enum Valores codigos;
char estado_actual[30];

char estados_actuadores[4];			// Guardo estados e actuadores para enviar al bluetooth
									// intractor, extractor, luz, calentador

/*==================[declaraciones de funciones internas]====================*/




/*==================[declaraciones de funciones externas]====================*/

