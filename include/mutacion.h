#ifndef MUTACION_H
#define MUTACION_H

#include "solucion.h"

// Las tres mutaciones reciben el individuo por copia y devuelven la copia mutada y ya evaluada:
// el original no se modifica.

/**
 * Cada depósito (el de inicio y el de retorno de cada trip) se muta con probabilidad probMutar.
 * Inicio: sorteo uniforme entre los depósitos de salida permitidos (exploratorio).
 * Retorno: sorteo entre los factibles con los pesos de tablaCierres más el piso (influencia del
 * espacio de creencias normativo); antes de generacionSesgo el sorteo es uniforme.
 */
Individuo mutarDeposito(const Instancia& inst, const Parametros& p, Individuo ind, const std::vector<std::vector<double>>& tablaCierres, int generacion, int generacionSesgo, double probMutar, double piso, std::mt19937& rng);

/**
 * Por cada vehículo, con probabilidad probMutar, elige entre 1 y la mitad de sus trips
 * (redondeada hacia arriba) y baraja los clientes dentro de cada uno.
 */
Individuo mutarClientes(const Instancia& inst, const Parametros& p, Individuo ind, double probMutar, std::mt19937& rng);

/**
 * Por cada vehículo con al menos dos trips, con probabilidad probMutar, intercambia dos trips
 * completos (clientes y depósito de retorno) y corrige el cierre de la route si hace falta.
 */
Individuo intercambiarTrips(const Instancia& inst, const Parametros& p, Individuo ind, double probMutar, std::mt19937& rng);

#endif // MUTACION_H
