#ifndef SOLUCION_H
#define SOLUCION_H

#include <vector>
#include <random>
#include "instancia.h"



struct Evaluacion {
    int tiempo;
    int excesoCarga;   // sumatoria  max(0, carga_trip − cap) sobre todos los trips
    int excesoTiempo;  // sumatoria max(0, tiempo_route − tMax) sobre todas las routes

    // Aborta con mensaje si la solución usa un arco inexistente (time == INF)
    bool factible() const { return excesoCarga == 0 && excesoTiempo == 0; }
};

/**
 * Un trip: clientes en orden de visita y depósito al que retorna.
 * Solamente señala desde donde vuelve.
 */
struct Trip {
    std::vector<int> clientes;
    int retorno;
};

/**
 * Esta estructura marca el inicio del Route y la coleccion de Trips que tiene.
 */
struct Route {
    int inicio;
    std::vector<Trip> trips;
};

struct Parametros {
    double rcl; // Restricted candidate list factor
    int maxTrips; // Cantidad máxima de trips
    double lambdaCap, lambdaT; // Pesos que castigan Capacidad y Tiempo excedido
    int poblacion; // Tamaño de la población
    unsigned semilla; // It is what it is.
};

// Una solución tiene una route por vehículo; el índice es el vehículo.
using Solucion = std::vector<Route>;

/**
 * Un individuo en la poblacion del algoritmo cultural de esta implementacion
 * debe estar dotado de qué solución exhibe, la capacidad de ser evaluado y el
 * fitness que este exhibe. Es el que exhibe la solucion completa del problema
 * es decir dice cuantos vehículos se usan, etc.
 */
struct Individuo {
    Solucion sol;
    Evaluacion eval;
    double fitness;
};

// ---------------------------------------------------------
// --------------- Declaracion de prototipos ---------------
// ---------------------------------------------------------

/**
 * Recorre todas las routes y acumula tiempo y excesos.
 * Aborta con mensaje si la solución usa un arco inexistente (time == INF)
 */
Evaluacion evaluar(const Instancia& inst, const Solucion& sol);

// Penalización lineal: tiempo + lambdaCap·excesoCarga + lambdaT·excesoTiempo.
double fitness(const Evaluacion& e, double lambdaCap, double lambdaT);

/**
 * En esta funcion se inicializa la solución inicial, en nuestro caso se implementa la funcion
 * propuesta en el paper original de FDMTVRP.
 */
Solucion inicializarSolucion(const Instancia& inst, const Parametros& p, std::mt19937& rng);

/**
 * TODO: Implementar cuando el algoritmo cultural se tenga que cocinar.
 */
std::vector<Solucion> inicializarPoblacion(const Instancia& inst, const Parametros& p, std::mt19937& rng);

// Imprime cada route con IDs de la instancia (inicio, clientes, depósito de retorno de cada trip)
// y la evaluación: tiempo, excesos y factibilidad.
void imprimirSolucion(const Instancia& inst, const Solucion& sol);

#endif // SOLUCION_H
