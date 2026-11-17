#ifndef CREENCIAS_H
#define CREENCIAS_H

#include "solucion.h"

using namespace std;
// Ambas funciones esperan la población ordenada de menor a mayor fitness.

/**
 * Espacio de creencias normativo. Primero olvida: multiplica toda la tabla por p.cierresOlvido.
 * Después aprende: por cada trip de la p.cierresFraccion mejor de la poblacion suma 1 en la casilla
 * [último cliente][depósito de retorno]
 */
void actualizarTablaCierres(vector<vector<double>>& tablaCierres, const vector<Individuo>& poblacion, const Parametros& p);

/**
 * Espacio de creencias circunstancial. Agrega al archivo la p.mejoresFraccion mejor de la poblacion,
 * descarta los repetidos (mismo fitness) y conserva los p.mejoresTam mejores, ordenados.
 */
void actualizarMejores(vector<Individuo>& mejores, const vector<Individuo>& poblacion, const Parametros& p);
#endif // CREENCIAS_H
