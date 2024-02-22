#include "sapi.h"
#include "sensorAgua.h"

bool_t sensorAguaMain(){
	return gpioRead( LCDEN );
}


void sensorAguaConfig(){
	gpioInit( LCDEN, GPIO_INPUT_PULLUP );
}
