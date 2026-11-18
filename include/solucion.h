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

/**
 * Parámetros del algoritmo, agrupados por el componente que los usa.
 * Convención de nombres: prob* son probabilidades; el prefijo cierres* pertenece a la tabla de
 * cierres (espacio de creencias normativo) y mejores* al archivo de mejores (circunstancial).
 */
struct Parametros {
    // --- Ejecución ---
    unsigned semilla;          // Semilla del generador aleatorio.
    int poblacion;             // Tamaño de la población.
    int nGeneraciones;         // Máximo de generaciones (criterio de término).

    // --- Restricciones y evaluación ---
    int maxTrips;              // Máximo de trips por route; 0 = sin límite.
    double lambdaCap, lambdaT; // Pesos que castigan el exceso de carga y el exceso de tiempo.

    // --- Inicialización ---
    double rcl;                // Fracción de candidatos más cercanos entre los que sortea el greedy.

    // --- Cruce ---
    double probCruzamiento;    // Probabilidad de que el hijo nazca de cruzarTrips (si no, copia del padre 1).

    // --- Mutaciones ---
    double probMutDeposito;    // mutarDeposito: probabilidad de mutar cada depósito.
    double probMutClientes;    // mutarClientes: probabilidad de mutar cada vehículo.
    double probMutIntercambio; // intercambiarTrips: probabilidad de mutar cada vehículo.

    // --- Espacio de creencias normativo: tabla de cierres ---
    double cierresFraccion;    // Fracción mejor de la población que alimenta la tabla en cada generación.
    double cierresOlvido;      // Factor (entre 0 y 1) por el que se multiplica la tabla antes de sumar.
    double cierresPiso;        // Peso mínimo de cada depósito candidato al sortear un retorno.
    int cierresGenSesgo;       // Generación desde la que mutarDeposito sortea con la tabla; antes, uniforme.

    // --- Espacio de creencias circunstancial: archivo de mejores ---
    double mejoresFraccion;    // Fracción mejor de la población que postula al archivo en cada generación.
    int mejoresTam;            // Tamaño máximo del archivo.
    double mejoresProb;        // Probabilidad de que el padre 2 salga del archivo y no de la población.
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
 * Crea p.poblacion individuos: cada uno se construye con inicializarSolucion y se evalúa al nacer,
 * de modo que sol, eval y fitness quedan coherentes. No ordena la población.
 */
std::vector<Individuo> inicializarPoblacion(const Instancia& inst, const Parametros& p, std::mt19937& rng);

/**
 * Si el último trip de la route del vehículo k vuelve a un depósito donde el vehículo no puede
 * terminar, lo cambia por el depósito permitido más cercano a su último cliente.
 * Lo usan el cruce y las mutaciones que pueden cambiar cuál es el último trip.
 */
void corregirCierre(const Instancia& inst, int k, Route& route);

// Imprime cada route con IDs de la instancia (inicio, clientes, depósito de retorno de cada trip)
// y la evaluación: tiempo, excesos y factibilidad.
void imprimirSolucion(const Instancia& inst, const Solucion& sol);

#endif // SOLUCION_H
