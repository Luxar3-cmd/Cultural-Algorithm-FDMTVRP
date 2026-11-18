#ifndef PARAMETROS_H
#define PARAMETROS_H

#include "solucion.h"

/**
 * Lee los parámetros del algoritmo desde la línea de comandos:
 *
 *     ./main <instancia.dat> [--nombre valor] ...
 *
 * Cada bandera se llama igual que su campo en el struct Parametros (por ejemplo --poblacion 200).
 * Los parámetros que no se entregan toman su valor por defecto. Si no se entrega --semilla, se
 * sortea una. No valida la entrada: asume banderas y valores correctos.
 */
Parametros leerParametros(int argc, char* argv[]);

// Muestra cómo se ejecuta el programa y todas las banderas con su valor por defecto.
void imprimirUso(const char* programa);

// Imprime los valores con que se está ejecutando, con el mismo nombre de sus banderas.
void imprimirParametros(const Parametros& p);

#endif // PARAMETROS_H
