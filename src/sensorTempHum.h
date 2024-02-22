/*
DHT11 connections:
                                                DHT11
                                           +--------------+
             EDU-CIAA-NXP +3.3V <--> 1 ----| +++  +++  +++|
   EDU-CIAA-NXP GPIO1 (pull-up) <--> 2 ----| +++  +++  +++|
                 (SIN CONEXION) <--> 3 ----| +++  +++  +++|
               EDU-CIAA-NXP GND <--> 4 ----| +++  +++  +++|
                                           +--------------+

                                           DHT11 on a board
                                          +----------------+
                                          |    +-----------+--+
             EDU-CIAA-NXP GPIO1 <--> S ----o --| +++  +++  +++|
             EDU-CIAA-NXP +3.3V <--> + ----o --| +++  +++  +++|
              EDU-CIAA-NXP GND  <--> - ----o --| +++  +++  +++|
                                          |  --| +++  +++  +++|
                                          |    +-----------+--+
                                          +----------------+
*/

/*===========================================================================*/
#ifndef SENSORTEMPHUM_H
#define SENSORTEMPHUM_H

#include "sapi.h"        // <= Inclusion de la Biblioteca sAPI

void dht11Main(  float * );

#endif
