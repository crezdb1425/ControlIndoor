/* Copyright 2018, Eric Pernia
 * All rights reserved.
 *
 * This file is part of sAPI Library.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIA FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "sapi.h"        // <= Biblioteca sAPI
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "bluetooth.h"
#include "display.h"
#include "sensorTempHum.h"
#include "sensorAgua.h"
#include "prueba.h"


//int cont_ms50 = 0;

uint8_t data = 0;

/*==================[funcion principal]======================================*/

// FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE ENCENDIDO O RESET.
int main( void )

{
   // ---------- CONFIGURACIONES E INICIALIZACIONES ------------------------------ //

	/* Placa */
	boardConfig();
	uartConfig( UART_USB, 115000 ); // Inicializar periferico UART_USB

	hc06Config();				// Inicializo bluetooth
	displayInit();				// Inicializo display
	dht11Init( SENSORHYT ); 	// Inicializo el sensor DHT11 Temp Hum (funciona a 5V)
	sensorAguaConfig();			// Inicializo sensor nivel de agua

	/*  LEDS  */
	gpioInit( LEDVERDE, GPIO_OUTPUT );
	gpioInit( LEDROJO, GPIO_OUTPUT );
	gpioInit( LEDAMARILLO, GPIO_OUTPUT );


	/* RELES */
	gpioInit( RELE2B, GPIO_OUTPUT );
	gpioWrite( RELE2B, ON);
	gpioInit( RELE2A, GPIO_OUTPUT );
	gpioWrite( RELE2A, ON );
	gpioInit( RELE1A, GPIO_OUTPUT );
	gpioWrite( RELE1A, ON );
	gpioInit( RELE1B, GPIO_OUTPUT );
	gpioWrite( RELE1B, ON );

	gpioWrite( LEDROJO, ON ); // LED ROJO se enciende cuando comienza a funcionar

	char comenzar = TRUE;

	printf("COMENZAMOS \n");

	char textoInicial[] = "inicio";
	recibido = FALSE;
	uartInterrupt(UART_BLUETOOTH, TRUE);	// Habilito todas las interrupciones de UART_BLUETOOTH

	/* 	ME QUEDO ESPERANDO RECIBIR DEL BLUETOOTH POR PRIMERA VEZ
	 * 	Recibo "inicio" y luego la hora actual tomada del celular a traves de la app
	 * 	y los valores de temp, hum y horas de luz para comenzar */
	while(comenzar) {
	  recibido = waitForReceiveStringOrTimeoutBlocking(
					UART_BLUETOOTH,
					textoInicial,
					strlen(textoInicial),
					6000
				 );

	  /* Si recibe el string almacenado en textoInicial indica que llego el
	   * mensaje esperado. */
	  if( recibido ){
		 uartWriteString( UART_USB, "\r\nLlego el mensaje esperado\r\n" );
		 comenzar = FALSE; 		// Salgo del while
	  }
	  /* Si no lo recibe indica que salio de la funcion
	   * waitForReceiveStringOrTimeoutBlocking  por timeout. */
	  else{
		 uartWriteString( UART_USB, "\r\nSalio por timeout\r\n" );
	  }

	}
	char horaBuffer [9];
	bool_t retVal = receiveBytesUntilReceiveStringOrTimeoutBlocking(
					  UART_BLUETOOTH,
					  "HOK\r\n", strlen("HOK\r\n"),
					  horaBuffer, &horaBuffer,
					  10000
				   );
	if (retVal){
		uartWriteString( UART_USB, "\r\nrecibi la hora!!\r\n" );
	}

	char flagParams[] = "paramsModif";
	char paramsBuffer [27];
	recibido = FALSE;
	comenzar = TRUE;
	while(comenzar) {
	  recibido = waitForReceiveStringOrTimeoutBlocking(
					UART_BLUETOOTH,
					flagParams,
					strlen(flagParams),
					6000
				 );

	  /* Si recibe el string almacenado en textoInicial indica que llego el
	   * mensaje esperado. */
	  if( recibido ){
		 uartWriteString( UART_USB, "Mandame los parametros \r\n" );
		 comenzar = FALSE; 		// Salgo del while
	  }
	  /* Si no lo recibe indica que salio de la funcion
	   * waitForReceiveStringOrTimeoutBlocking  por timeout. */
	  else{
		 uartWriteString( UART_USB, "\r\nSalio por timeout\r\n" );
	  }
	}

	retVal = receiveBytesUntilReceiveStringOrTimeoutBlocking(
						  UART_BLUETOOTH,
						  "POK\r\n", strlen("POK\r\n"),
						  paramsBuffer, &paramsBuffer,
						  10000
					   );
	if (retVal){
		uartWriteString( UART_USB,  "\r\nrecibi los parametros!!\r\n" );
	}

	inicializacion(horaBuffer, paramsBuffer);


	// Inicializar el conteo de Ticks con resolucion de 1000 ms (se ejecuta
	// periodicamente una interrupcion cada 1000 ms que incrementa un contador
	// de Ticks obteniendose una base de tiempos).
	//tickInit( 50 );			// MAXIMA resolucion de 50ms
	//tickCallbackSet( scheduleAndDispatchTasks, NULL );  // Funcion que se llama cada tick

	strcpy(estado_actual, "CONTROLADO");

	uint8_t minuto=6;

	while ( TRUE ){
		   if (minuto == 6){
			   scheduleAndDispatchTasks();
			   minuto = 0;
		   }
		   if (strcmp(estado_actual,"CONTROLADO") == 1){
			   if(strcmp(estado_actual,"SUBIENDO_HUM") == 0 && strcmp(val.agua,"AC") == 1)
				   break;
			   else
				   gpioWrite(LEDVERDE, ON);
		   }
		   gpioWrite(LEDROJO, OFF);
		   delay(1000);
		   gpioWrite(LEDROJO, ON);
		   gpioWrite(LEDVERDE, OFF);
		   delay(1000);
		   minuto++;
	   }

   // NO DEBE LLEGAR NUNCA AQUI, debido a que a este programa se ejecuta
   // directamenteno sobre un microcontroladore y no es llamado por ningun
   // Sistema Operativo, como en el caso de un programa para PC.
   return 0;
}

/*==================[definiciones de funciones internas]=====================*/

// Funcion que recibe la hora actual que envia el bluetooth e inicializa
// Variables ----- parametros=tMax,tMin,hMax,hMin,hsInicio,hsFin
// actualizo variable hora_inicio; hora_fin, cotas_actuales; parametros_cargados, luz_faltan
void inicializacion(char hora[], char parametros[]){
	hsConvert(hora);
	actualizar_valores(parametros);
}


// Funcion que convierte los valores recibidos de char a float y establece
// valores de temp, hum, horas de luz y oscuridad para el funcionamiento
// del sistema.
void actualizar_valores(char parametros[]){
	printf("Adentro actualizar_valores\r\n");
	char aux[6] = "";
	char aux1[3] = "";
	char temp[2];
	temp[1] = '\0';
	int j = 4;		// Empieza en 4 porq las posiciones anteriores son basura
	for (int  i=0; i<6; i++){
		while (parametros[j] != ',' && j < 27){
			temp[0] = parametros[j]; 		// Convertir el carácter en cadena
			strcat(aux, temp); 						// Concatenar la cadena temp a aux
			j++;
		}
		j++;
		if(i<4){
			parametros_cargados[i] = atoi(aux);			// convierto a float los parametros
			parametros_actuales[i] = parametros_cargados[i];
		}
		else{
			if(i == 4){				// guardo hora inicio y fin
				for (int  m=0; m<2; m++){
					temp[0] = aux[m]; 		// Convertir el carácter en cadena
					strcat(aux1, temp); 						// Concatenar la cadena temp a aux
				}
				aux1[2] = '\0';
				hora_inicio.hora = atoi(aux1);
				strcpy(aux1, "");
				for (int  p=3; p<5; p++){
					temp[0] = aux[p]; 		// Convertir el carácter en cadena
					strcat(aux1, temp); 						// Concatenar la cadena temp a aux
				}
				aux1[2] = '\0';
				hora_inicio.minuto = atoi(aux1);
			}
			else{
				strcpy(aux1, "");
				for (int  m=0; m<2; m++){
					temp[0] = aux[m]; 		// Convertir el carácter en cadena
					strcat(aux1, temp); 						// Concatenar la cadena temp a aux
				}
				aux1[2] = '\0';
				hora_fin.hora = atoi(aux1);
				strcpy(aux1, "");
				for (int  p=3; p<5; p++){
					temp[0] = aux[p]; 		// Convertir el carácter en cadena
					strcat(aux1, temp); 						// Concatenar la cadena temp a aux
				}
				aux1[2] = '\0';
				hora_fin.minuto = atoi(aux1);
			}
		}
		strcpy(aux,"");
	}
	cotas_actuales[0] = parametros_cargados[0] + 1;		// TempMax - Cota superior
	cotas_actuales[1] = parametros_cargados[0] - 1;		// TempMax - Cota inferior
	cotas_actuales[2] = parametros_cargados[1] + 1;		// TempMin - Cota superior
	cotas_actuales[3] = parametros_cargados[1] - 1;		// TempMin - Cota inferior
	cotas_actuales[4] = parametros_cargados[2] + 2;		// HumMax - Cota superior
	cotas_actuales[5] = parametros_cargados[2] - 2;		// HumMax - Cota inferior
	cotas_actuales[6] = parametros_cargados[3] + 2;		// HumMin - Cota superior
	cotas_actuales[7] = parametros_cargados[3] - 2;		// HumMin - Cota inferior
	printf("\r\ncotas_actuales: ");
	for(int i=0; i<8; i++){
		printf(" %d", cotas_actuales[i]);
	}
	printf("\r\n");
}



// Funcion que convierte la hora recibida como cadena de caracteres
// a un arreglo de enteros
void hsConvert(char h[]){
	char hora[3], minuto[3];
	for (int  i=0; i<2; i++){
		hora[i] = h[i+4];		// Las primeras 4 posiciones son basura
		minuto[i] = h[i+7];
	}
	hora_actual.hora = atoi(hora);
	hora_actual.minuto = atoi(minuto);
}


// Funcion que controla los parametros y actualiza codigos del estado del indoor
void control_parametros(){
	if(sensado[1] > parametros_actuales[0]){  //Si temp MAYOR tMax
		strcpy(val.temp, "TA");
		printf("TA: sensado=%d > (tMax)parametros_actuales[0]=%d \r\n",(int)sensado[1],parametros_actuales[0] );
	}
	else
		if(sensado[1] < parametros_actuales[0]){	//Si temp MENOR tMax
			strcpy(val.temp, "TC");
			printf("TC: sensado=%d < (tMax)parametros_actuales[0]=%d \r\n",(int)sensado[1],parametros_actuales[0] );
		}
	if(sensado[0] > parametros_actuales[2]){  //Si hum MAYOR hMax
		strcpy(val.hum, "HA");
		printf("HA: sensado=%d > (hMax)parametros_actuales[2]=%d \r\n",(int)sensado[0],parametros_actuales[2] );
		if(strcmp(val.temp, "TA")==0){
			codigos = TAHA;
			printf("Codigo: TAHA\r\n ");
		}
		else{
			codigos = TCHA;
			printf("Codigo: TCHA\r\n ");
		}
	}
	else
		if(sensado[0] < parametros_actuales[3]){	//Si hum MENOR hMin
			strcpy(val.hum, "HB");
			printf("HB: sensado=%d < (hMin)parametros_actuales[3]=%d \r\n",(int)sensado[0],parametros_actuales[3] );
			if(strcmp(val.temp, "TA")==0){
				codigos = TAHB;
				printf("Codigo: TAHB\r\n ");
			}
			else{
				codigos = TCHB;
				printf("Codigo: TCHB\r\n ");
			}
		}
		else
			if(sensado[0] > parametros_actuales[3] && sensado[0] < parametros_actuales[2]){ //Si hum esta entre hMax y hMin
				strcpy(val.hum, "HC");
				printf("HC: (hMin)parametros_actuales[2]=%d < sensado=%d < (hMax)parametros_actuales[3]=%d \r\n",parametros_actuales[3],(int)sensado[0],parametros_actuales[2] );
				if (strcmp(val.temp, "TA") == 0){
					codigos = TAHC;
					printf("Codigo: TAHC\r\n ");
				}
				else{
					codigos = TCHC;
					printf("Codigo: TCHC\r\n ");
				}
			}
	if(sensorAguaMain())				//Si nivelAgua correcto
		strcpy(val.agua, "AC");
	else
		strcpy(val.agua, "AB");
}


// Funcion que determina accion a seguir segun los valores
void estados_indoor(){
	switch (codigos){
	case TAHA:
		if(strcmp(estado_actual, "BAJANDO_TEMP_Y_HUM")==0)
			break;
		else{
			// Encender intractor
			gpioWrite( RELE1A, OFF);
			estados_actuadores[0] = '1';
			// Apago resto de actuadores
			gpioWrite( RELE1B, ON);
			gpioWrite( RELE2A, ON);
			gpioWrite( RELE2B, ON);
			estados_actuadores[1] = '0';
			estados_actuadores[2] = '0';
			estados_actuadores[3] = '0';
			char temp[] = "BAJANDO_TEMP_Y_HUM";
			strcpy(estado_actual, temp);
			parametros_actuales[0] = cotas_actuales[1];  // tMax=tMaxInf
			parametros_actuales[2] = cotas_actuales[5];	// hMax=hMaxInf
			parametros_actuales[1] = parametros_cargados[1];
			parametros_actuales[3] = parametros_cargados[3];
		};
		break;
	case TAHB:
		if(strcmp(estado_actual, "BAJANDO_TEMP_Y_SUB_HUM")==0)
			break;
		else{
			// Encender intractor
			gpioWrite( RELE1A, OFF);
			estados_actuadores[0] = '1';
			if(strcmp(val.agua, "AC")==0){
				// Encender calentador
				gpioWrite( RELE1B, OFF);
				estados_actuadores[3] = '1';
			}
			// Apagar el resto de actuadores
			gpioWrite( RELE2A, ON);
			gpioWrite( RELE2B, ON);
			estados_actuadores[1] = '0';
			estados_actuadores[2] = '0';
			char temp[] = "BAJANDO_TEMP_Y_SUB_HUM";
			strcpy(estado_actual, temp);
			parametros_actuales[0] = cotas_actuales[1];  // tMax=tMaxInf
			parametros_actuales[3] = cotas_actuales[6];	// hMin=hMinSup
			parametros_actuales[1] = parametros_cargados[1];
			parametros_actuales[2] = parametros_cargados[2];
		};
		break;
	case TAHC:
		if(strcmp(estado_actual, "BAJANDO_TEMP")==0)
			break;
		else{
			// Encender extractor e intractor
			gpioWrite( RELE1A, OFF);
			gpioWrite( RELE2A, OFF);
			estados_actuadores[0] = '1';
			estados_actuadores[1] = '1';
			// Apagar resto de actuadores
			gpioWrite( RELE1B, ON);
			gpioWrite( RELE2B, ON);
			estados_actuadores[2] = '0';
			estados_actuadores[3] = '0';
			char temp[] = "BAJANDO_TEMP";
			strcpy(estado_actual, temp);
			parametros_actuales[0] = cotas_actuales[1];  // tMax=tMaxInf
			parametros_actuales[1] = parametros_cargados[1];
			parametros_actuales[2] = parametros_cargados[2];
			parametros_actuales[3] = parametros_cargados[3];
		};
		break;
	case TCHA:
		if(strcmp(estado_actual, "BAJANDO_HUM")==0)
			break;
		else{
			// Encender extractor
			gpioWrite( RELE2A, OFF);
			estados_actuadores[1] = '1';
			// Apagar resto de actuadores
			gpioWrite( RELE1A, ON);
			gpioWrite( RELE1B, ON);
			gpioWrite( RELE2B, ON);
			estados_actuadores[0] = '0';
			estados_actuadores[2] = '0';
			estados_actuadores[3] = '0';
			char temp[] = "BAJANDO_HUM";
			strcpy(estado_actual, temp);
			parametros_actuales[0] = parametros_cargados[0];
			parametros_actuales[1] = parametros_cargados[1];
			parametros_actuales[2] = cotas_actuales[5];	// hMax=hMaxInf
			parametros_actuales[3] = parametros_cargados[3];
		}
		break;
	case TCHB:
		if(strcmp(estado_actual, "SUBIENDO_HUM")==0)
			break;
		else{
			if(strcmp(val.agua, "AC") == 0){
				// Encender calentador
				printf("ENTRO AL IF DE AC\r\n");
				gpioWrite( RELE1B, OFF);
				estados_actuadores[3] = '1';
			}
			// Apagar resto de actuadores
			gpioWrite( RELE1A, ON);
			gpioWrite( RELE2A, ON);
			gpioWrite( RELE2B, ON);
			estados_actuadores[0] = '0';
			estados_actuadores[1] = '0';
			estados_actuadores[2] = '0';
			char temp[] = "SUBIENDO_HUM";
			strcpy(estado_actual, temp);
			parametros_actuales[0] = parametros_cargados[0];
			parametros_actuales[1] = parametros_cargados[1];
			parametros_actuales[3] = cotas_actuales[6];	// hMin=hMinSup
			parametros_actuales[2] = parametros_cargados[2];
		}
		break;
	default:
		// Apagar todos los actuadores
		gpioWrite( RELE1A, ON);
		gpioWrite( RELE1B, ON);
		gpioWrite( RELE2A, ON);
		gpioWrite( RELE2B, ON);
		estados_actuadores[0] = '0';
		estados_actuadores[1] = '0';
		estados_actuadores[2] = '0';
		estados_actuadores[3] = '0';
		char temp[] = "CONTROLADO";
		strcpy(estado_actual, temp);
		parametros_actuales[0] = parametros_cargados[0];
		parametros_actuales[1] = parametros_cargados[1];
		parametros_actuales[2] = parametros_cargados[2];
		parametros_actuales[3] = parametros_cargados[3];
	}
	if(strcmp(estado_actual, "CONTROLADO")==1)	// Si hay actuadores encendidos
		// Encender led verde
		gpioWrite( LEDVERDE, ON);
	else
		gpioWrite( LEDVERDE, OFF);
	if(strcmp(val.agua, "AB")==1)
		// Encender led amarillo
		gpioWrite( LEDAMARILLO, OFF );
	else
		gpioWrite( LEDAMARILLO, ON );
	printf("\r\nAGUA: %s", val.agua);
	printf("\r\nESTADO ACTUAL: ");
	for(int i=0; i<strlen(estado_actual); i++){
		printf("%c", estado_actual[i]);
	}
	printf("\r\n");
}


// Funcion que contiene el planificador y despachador de tareas.
void scheduleAndDispatchTasks()
{
	//if(cont_ms50 == 600){		// 80 = cada 4seg
		//cont_ms50 = 0;
    if(hora_fin.hora < hora_inicio.hora){   // La hora de fin es al otro dia
        // Casos donde la luz se encuentra encendida - calculo horas que faltan para que se apague
        if(hora_actual.hora < hora_fin.hora){
            luz_faltan.dia = TRUE;
            gpioWrite(RELE2B, OFF);
            if(hora_actual.minuto < hora_fin.minuto)
                convertirMinutosAHoras((hora_fin.hora - hora_actual.hora)*60 + hora_fin.minuto - hora_actual.minuto);
            else
                convertirMinutosAHoras((hora_fin.hora - hora_actual.hora)*60 - hora_actual.minuto - hora_fin.minuto);
            goto stop;
        }
        if(hora_actual.hora == hora_fin.hora && hora_actual.minuto < hora_fin.minuto){
            luz_faltan.dia = TRUE;
            gpioWrite(RELE2B, OFF);
            luz_faltan.horas = 0;
            luz_faltan.minutos = hora_fin.minuto - hora_actual.minuto;
            goto stop;
        }
        if(hora_actual.hora > hora_inicio.hora || (hora_actual.hora == hora_inicio.hora && hora_actual.minuto >= hora_inicio.minuto)){
            luz_faltan.dia = TRUE;
            gpioWrite(RELE2B, OFF);
            if(hora_actual.minuto < hora_fin.minuto)
                convertirMinutosAHoras((24 - hora_actual.hora + hora_fin.hora)*60 + hora_fin.minuto - hora_actual.minuto);
            else
                convertirMinutosAHoras((24 - hora_actual.hora + hora_fin.hora)*60 - hora_actual.minuto - hora_fin.minuto);
            goto stop;
        }
        // Casos donde la luz esta apagada - calculo horas que faltan para que se encienda
        luz_faltan.dia = FALSE;
        gpioWrite(RELE2B, ON);
        if(hora_actual.hora > hora_fin.hora){
            if(hora_actual.hora < hora_inicio.hora && hora_actual.minuto < hora_inicio.minuto)
                convertirMinutosAHoras((hora_inicio.hora - hora_actual.hora)*60 + hora_inicio.minuto - hora_actual.minuto);
            else
                if(hora_actual.hora == hora_inicio.hora){
                    luz_faltan.horas = 0;
                    if(hora_actual.minuto < hora_inicio.minuto){        // por las dudas, pero deberia ser
                        luz_faltan.minutos = hora_inicio.minuto - hora_actual.minuto;
                    }
                }
            goto stop;
        }
        if(hora_actual.hora == hora_fin.hora){
            if(hora_actual.minuto < hora_inicio.minuto) // por las dudas, pero deberia ser si llega aca
                convertirMinutosAHoras((hora_inicio.hora - hora_actual.hora)*60 + hora_inicio.minuto - hora_actual.minuto);
        }

    }
    else{   // La hora de fin es el mismo dia que inicio
        if((hora_actual.hora < hora_fin.hora || (hora_actual.hora == hora_fin.hora && hora_actual.minuto < hora_fin.minuto)) && (hora_actual.hora > hora_inicio.hora || (hora_actual.hora == hora_inicio.hora && hora_actual.minuto >= hora_inicio.minuto))){
            luz_faltan.dia = TRUE;
            gpioWrite(RELE2B, OFF);
            if(hora_actual.hora < hora_fin.hora){
                luz_faltan.dia = TRUE;
                if(hora_actual.minuto < hora_fin.minuto)
                    convertirMinutosAHoras((hora_fin.hora - hora_actual.hora)*60 + hora_fin.minuto - hora_actual.minuto);
                else
                	convertirMinutosAHoras(((hora_fin.hora - hora_actual.hora)*60 - hora_actual.minuto - hora_fin.minuto));
                goto stop;
            }
            if((hora_actual.hora == hora_fin.hora && hora_actual.minuto < hora_fin.minuto) && (hora_actual.hora > hora_inicio.hora || (hora_actual.hora == hora_inicio.hora && hora_actual.minuto <= hora_inicio.minuto))){
                if(hora_actual.hora == hora_fin.hora && hora_actual.minuto < hora_fin.minuto){
                    luz_faltan.horas = 0;
                    luz_faltan.minutos = hora_fin.minuto - hora_actual.minuto;
                    goto stop;
                }
                if(hora_actual.hora > hora_inicio.hora){
                    if(hora_actual.minuto < hora_fin.minuto)
                        convertirMinutosAHoras((hora_fin.hora - hora_actual.hora)*60 + hora_fin.minuto - hora_actual.minuto);
                    else
                        convertirMinutosAHoras((hora_fin.hora - hora_actual.hora)*60 - hora_actual.minuto - hora_fin.minuto);
                    goto stop;
                }
                if(hora_actual.hora == hora_inicio.hora && hora_actual.minuto <= hora_inicio.minuto){
                    luz_faltan.horas = 0;
                    luz_faltan.minutos = hora_inicio.minuto - hora_actual.minuto;
                    goto stop;
                }
            }
        }
        else{
            luz_faltan.dia = FALSE;
            gpioWrite(RELE2B, ON);
            if(hora_actual.hora == hora_fin.hora || (hora_actual.hora > hora_fin.hora && hora_actual.minuto > hora_fin.minuto)){
                if(hora_actual.hora == hora_fin.hora){
                    if(hora_actual.minuto < hora_inicio.minuto)
                        convertirMinutosAHoras((24 - hora_actual.hora + hora_inicio.hora)*60 + hora_inicio.minuto - hora_actual.minuto);
                    else
                        convertirMinutosAHoras((24 - hora_actual.hora + hora_inicio.hora)*60 - hora_inicio.minuto - hora_actual.minuto);
                    goto stop;
                }
            }
        }
    }
    stop: printf("HORA ACTUAL: %d:%d\r", hora_actual.hora, hora_actual.minuto);
	if ( hora_actual.minuto++ == 60){
		hora_actual.minuto = 0;
		if( hora_actual.hora++ == 24 ){
			hora_actual.hora = 0;
		}
	}
	dht11Main(sensado);					// Leo humedad y temperatura
	displayMain(sensado, luz_faltan);	// Le envio los datos a mostrar
	control_parametros();		// Chequeo estado del indoor segun valores actuales y cotas determinadas por usuario
	estados_indoor();					// Evaluo encender/apagar actuadores
	hc06Main(data, sensado, luz_faltan.horas, luz_faltan.minutos, luz_faltan.dia, estados_actuadores, val.agua);	// Si esta conectado bluetooth, envio datos
	//}
	//else{
		//cont_ms50++;
	//}
}

// Funcion que recibe minutos y lo convierte en horas
void convertirMinutosAHoras(int minutos) {
    // Calcular las horas y minutos
    int horas = minutos / 60;
    int minutosRestantes = minutos % 60;

    luz_faltan.horas = horas;
    luz_faltan.minutos = minutosRestantes;
}
/*==================[definiciones de funciones externas]=====================*/



/*==================[fin del archivo]========================================*/
