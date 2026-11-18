#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "instancia.h"
#include "solucion.h"
#include "parametros.h"
#include "cruzamiento.h"
#include "mutacion.h"
#include "creencias.h"

using namespace std;

/**
 * Guarda en mejorFactible al mejor individuo factible visto en toda la ejecución.
 * Recibe la población ordenada por fitness: en un individuo factible el fitness es solo su
 * tiempo, así que el primer factible que aparece es el mejor factible de esa generación.
 */
void registrarMejorFactible(const vector<Individuo>& poblacion, Individuo& mejorFactible, bool& hayFactible) {
    auto it = find_if(poblacion.begin(), poblacion.end(),
                      [](const Individuo& ind) { return ind.eval.factible(); });
    if (it == poblacion.end()) return; // Ningún factible en esta generación.
    if (!hayFactible || it->fitness < mejorFactible.fitness) {
        mejorFactible = *it;
        hayFactible = true;
    }
}

int main(int argc, char* argv[]) {

    // ---------------------------------------------------------
    // 1. Línea de comandos
    //    argv[1] es la instancia; el resto son parámetros de la forma --nombre valor.
    // ---------------------------------------------------------
    if (argc < 2 || string(argv[1]) == "--help") {
        imprimirUso(argv[0]);
        return argc < 2 ? 1 : 0;
    }

    // ---------------------------------------------------------
    // 2. Parámetros del algoritmo e instancia
    //    Los parámetros se leen primero: si alguno está mal, se avisa antes de cargar la instancia.
    //    Los valores por defecto y la validación están en src/parametros.cpp.
    // ---------------------------------------------------------
    Parametros p = leerParametros(argc, argv);

    Instancia inst;
    inicializarInstancia(inst, argv[1]);
    imprimirResumen(inst);
    imprimirParametros(p);

    // ---------------------------------------------------------
    // 3. Semilla y generador aleatorio
    //    Un solo generador para toda la ejecución. La semilla viene de --semilla o fue sorteada
    //    al leer los parámetros; con la misma semilla la ejecución se repite idéntica.
    // ---------------------------------------------------------
    mt19937 rng(p.semilla);
    auto inicioReloj = chrono::steady_clock::now(); // Para informar cuánto tardó el algoritmo.

    // ---------------------------------------------------------
    // 4. Población inicial
    //    Cada individuo se construye con el greedy pseudoaleatorio y se evalúa al nacer.
    // ---------------------------------------------------------
    vector<Individuo> Poblacion = inicializarPoblacion(inst, p, rng);

    // Se ordena de menor a mayor fitness: Poblacion[0] es el mejor individuo.
    sort(Poblacion.begin(), Poblacion.end(),
         [](const Individuo& a, const Individuo& b) { return a.fitness < b.fitness; });

    // ---------------------------------------------------------
    // 5. Espacio de creencias circunstancial
    //    Mejores: archivo con los mejores individuos distintos vistos en toda la ejecución,
    //    ordenado por fitness y de tamaño máximo p.mejoresTam. Parte con la mejor fracción
    //    de la población inicial.
    // ---------------------------------------------------------
    vector<Individuo> Mejores;
    actualizarMejores(Mejores, Poblacion, p);

    // ---------------------------------------------------------
    // 6. Espacio de creencias normativo
    //    TablaCierres[cliente][deposito]: cuántas veces un trip que termina en ese cliente
    //    volvió a ese depósito, contado por trip sobre la mejor fracción de la población.
    //    DISCLAIMER: una fila por nodo para indexar directo con el índice interno; las
    //    primeras n filas (con n la cantidad de depósitos) quedan sin uso a propósito.
    // ---------------------------------------------------------
    vector<vector<double>> TablaCierres(inst.idxToId.size(), vector<double>(inst.depots.size(), 0));
    actualizarTablaCierres(TablaCierres, Poblacion, p);

    // ---------------------------------------------------------
    // 7. Bucle de generaciones
    //    En cada generación todo individuo tiene un turno como padre 1: busca pareja, nace un
    //    hijo, el hijo muta y, si mejora al padre 1, ocupa su lugar en la generación siguiente.
    //    Termina al llegar a p.nGeneraciones o tras p.genSinMejora generaciones sin mejorar.
    // ---------------------------------------------------------
    bernoulli_distribution monedaCruce(p.probCruzamiento);
    bernoulli_distribution monedaMejores(p.mejoresProb); // Usado para ver si un padre será el utilizado del espacio de creencias para realizar el cruce.
    uniform_int_distribution<int> distPareja(0, p.poblacion - 2); // Posición del padre 2 entre los demás individuos.

    double mejorFitness = Poblacion[0].fitness; // Mejor fitness visto hasta ahora.
    int generacionesSinMejora = 0;              // Generaciones seguidas en que no bajó.
    int generacionesEjecutadas = 0;

    // Mejor individuo factible de toda la ejecución: es lo que se entrega al final.
    Individuo mejorFactible;
    bool hayFactible = false;
    registrarMejorFactible(Poblacion, mejorFactible, hayFactible);

    for (int gen = 0; gen < p.nGeneraciones; gen++) {
        // Los hijos se guardan aparte: todos los padres de una generación salen de la misma población.
        vector<Individuo> nextGen = Poblacion;
        uniform_int_distribution<int> distMejores(0, Mejores.size() - 1); // Posición dentro del archivo, que cambia de tamaño.

        for (int i = 0; i < p.poblacion; i++) {
            const Individuo& Padre1 = Poblacion[i];

            // Padre 2. Influencia del espacio circunstancial: con probabilidad p.mejoresProb sale
            // del archivo Mejores, al azar. Si no, es otro individuo de la población, distinto del padre 1.
            int j = distPareja(rng);
            if (j >= i) j++;
            const Individuo& Padre2 = monedaMejores(rng) ? Mejores[distMejores(rng)] : Poblacion[j]; 

            // Cruce: si la moneda no sale, el hijo parte como copia del padre 1.
            Individuo hijo = Padre1;
            if (monedaCruce(rng)) hijo = cruzarTrips(Padre1.sol, Padre2.sol, inst, p, rng);

            // Mutaciones: cada una decide adentro, con su propia moneda, si cambia algo.
            // mutarDeposito es la influencia del espacio normativo: sortea los retornos con TablaCierres.
            hijo = mutarDeposito(inst, p, hijo, TablaCierres, gen, rng);
            hijo = mutarClientes(inst, p, hijo, rng);
            hijo = intercambiarTrips(inst, p, hijo, rng);

            // Reemplazo: el hijo ocupa el lugar del padre 1 solo si lo mejora.
            if (hijo.fitness < Padre1.fitness) nextGen[i] = hijo;
        }

        Poblacion = nextGen;
        sort(Poblacion.begin(), Poblacion.end(),
             [](const Individuo& a, const Individuo& b) { return a.fitness < b.fitness; });

        // Aceptación: los mejores de la generación actualizan el espacio de creencias.
        actualizarTablaCierres(TablaCierres, Poblacion, p);
        actualizarMejores(Mejores, Poblacion, p);
        registrarMejorFactible(Poblacion, mejorFactible, hayFactible);
        generacionesEjecutadas++;

        cout << "Generacion " << gen << ": mejor fitness " << Poblacion[0].fitness << endl;

        // Criterio de término por estancamiento.
        if (Poblacion[0].fitness < mejorFitness) {
            mejorFitness = Poblacion[0].fitness;
            generacionesSinMejora = 0;
        } else {
            generacionesSinMejora++;
        }
        if (p.genSinMejora > 0 && generacionesSinMejora >= p.genSinMejora) {
            cout << "Termino: " << p.genSinMejora << " generaciones sin mejora." << endl;
            break;
        }
    }

    // ---------------------------------------------------------
    // 8. Resultado
    //    Se entrega la mejor solución factible vista. Si no hubo ninguna, la de menor fitness,
    //    avisando que no cumple capacidad o tiempo máximo.
    // ---------------------------------------------------------
    double segundos = chrono::duration<double>(chrono::steady_clock::now() - inicioReloj).count();
    const Individuo& entregado = hayFactible ? mejorFactible : Poblacion[0];

    cout << " --------------- Resultado ---------------" << endl;
    if (hayFactible) cout << "Mejor solucion factible encontrada:" << endl;
    else             cout << "No se encontro una solucion factible. Se entrega la de menor fitness:" << endl;
    imprimirSolucion(inst, entregado.sol);

    // Una sola línea con todo el resultado, pensada para leerla desde un script.
    cout << "RESULTADO factible=" << (hayFactible ? "si" : "no")
         << " tiempo=" << entregado.eval.tiempo
         << " excesoCarga=" << entregado.eval.excesoCarga
         << " excesoTiempo=" << entregado.eval.excesoTiempo
         << " fitness=" << entregado.fitness
         << " generaciones=" << generacionesEjecutadas
         << " segundos=" << segundos
         << " semilla=" << p.semilla << endl;

    return 0;
}
