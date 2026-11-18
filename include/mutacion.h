#ifndef MUTACION_H
#define MUTACION_H

#include "solucion.h"

// Las tres mutaciones reciben la solución por copia y devuelven la copia mutada: el original no se
// modifica. No evalúan: el bucle principal evalúa al hijo una sola vez, después de la última mutación.
// Sus probabilidades y demás ajustes se leen desde p.

/**
 * Cada depósito (el de inicio y el de retorno de cada trip) se muta con probabilidad p.probMutDeposito.
 * Inicio: sorteo uniforme entre los depósitos de salida permitidos (exploratorio).
 * Retorno: sorteo entre los factibles con los pesos de tablaCierres más p.cierresPiso (influencia
 * del espacio de creencias normativo); antes de la generación p.cierresGenSesgo el sorteo es uniforme.
 */
Solucion mutarDeposito(const Instancia& inst, const Parametros& p, Solucion sol, const std::vector<std::vector<double>>& tablaCierres, int generacion, std::mt19937& rng);

/**
 * Por cada vehículo, con probabilidad p.probMutClientes, elige entre 1 y la mitad de sus trips
 * (redondeada hacia arriba) y baraja los clientes dentro de cada uno.
 */
Solucion mutarClientes(const Instancia& inst, const Parametros& p, Solucion sol, std::mt19937& rng);

/**
 * Por cada vehículo con al menos dos trips, con probabilidad p.probMutIntercambio, intercambia dos trips
 * completos (clientes y depósito de retorno) y corrige el cierre de la route si hace falta.
 */
Solucion intercambiarTrips(const Instancia& inst, const Parametros& p, Solucion sol, std::mt19937& rng);

#endif // MUTACION_H
