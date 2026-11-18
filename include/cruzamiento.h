#ifndef CRUZAMIENTO_H
#define CRUZAMIENTO_H

#include "solucion.h"
using namespace std;

/**
 * Cruce por inserción: el hijo parte como copia de Padre2 y, por cada vehículo, recibe un trip
 * al azar de Padre1. Antes de insertarlo se borran sus clientes del hijo para no duplicarlos;
 * si la route no tiene cupo (maxTrips) esa inserción se descarta. Devuelve el hijo sin evaluar:
 * el bucle principal lo evalúa una sola vez, después de las mutaciones.
 */
Solucion cruzarTrips(const Solucion& Padre1, const Solucion& Padre2, const Instancia& inst, const Parametros& p, mt19937& rng);

#endif // CRUZAMIENTO_H
