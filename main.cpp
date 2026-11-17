#include <algorithm>
#include <iostream>
#include <random>
#include <vector>

#include "instancia.h"
#include "solucion.h"
#include "cruzamiento.h"
#include "mutacion.h"
#include "creencias.h"

using namespace std;

int main(int argc, char* argv[]) {

    // ---------------------------------------------------------
    // 1. Lectura de la instancia
    // ---------------------------------------------------------
    if (argc < 2) { cerr << "Uso: " << argv[0] << " <archivo.dat>" << endl; return 1; }
    Instancia inst;
    inicializarInstancia(inst, argv[1]);
    imprimirResumen(inst);

    // ---------------------------------------------------------
    // 2. Parámetros del algoritmo
    //    Agrupados por el componente que los usa, en el mismo orden que el struct Parametros.
    // ---------------------------------------------------------
    Parametros p;

    // Ejecución
    p.poblacion = 100;          // Tamaño de la población.
    p.nGeneraciones = 100;      // Cantidad de generaciones (criterio de término).

    // Restricciones y evaluación
    p.maxTrips = 3;             // Máximo de trips por route; 0 = sin límite.
    p.lambdaCap = 1;            // Castigo por unidad de carga sobre la capacidad.
    p.lambdaT = 1;              // Castigo por unidad de tiempo sobre tmax.

    // Inicialización
    p.rcl = 0.6;                // Fracción de candidatos más cercanos entre los que sortea el greedy.

    // Cruce
    p.probCruzamiento = 0.6;    // Probabilidad de que el hijo nazca de cruzarTrips.

    // Mutaciones
    p.probMutDeposito = 0.2;    // mutarDeposito: probabilidad de mutar cada depósito.
    p.probMutClientes = 0.2;    // mutarClientes: probabilidad de mutar cada vehículo.
    p.probMutIntercambio = 0.2; // intercambiarTrips: probabilidad de mutar cada vehículo.

    // Espacio de creencias normativo: tabla de cierres
    p.cierresFraccion = 0.2;    // Fracción mejor de la población que alimenta la tabla.
    p.cierresOlvido = 0.7;      // Factor por el que se multiplica la tabla en cada generación.
    p.cierresPiso = 0.1;        // Peso mínimo de cada depósito candidato al sortear un retorno.
    p.cierresGenSesgo = 20;     // Generación desde la que mutarDeposito sortea con la tabla.

    // Espacio de creencias circunstancial: archivo de mejores
    p.mejoresFraccion = 0.2;    // Fracción mejor de la población que postula al archivo.
    p.mejoresTam = 50;          // Tamaño máximo del archivo.

    // ---------------------------------------------------------
    // 3. Semilla y generador aleatorio
    //    Un solo generador para toda la ejecución; se imprime la semilla para poder repetirla.
    // ---------------------------------------------------------
    random_device rd;
    p.semilla = rd();
    mt19937 rng(p.semilla);

    cout << "---------------" << endl;
    cout << "Semilla inicializada: " << p.semilla << endl;
    cout << "---------------" << endl;

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
    // ---------------------------------------------------------
    bernoulli_distribution monedaCruce(p.probCruzamiento);
    uniform_int_distribution<int> distPareja(0, p.poblacion - 2); // Posición del padre 2 entre los demás individuos.

    for (int gen = 0; gen < p.nGeneraciones; gen++) {
        // Los hijos se guardan aparte: todos los padres de una generación salen de la misma población.
        vector<Individuo> nextGen = Poblacion;

        for (int i = 0; i < p.poblacion; i++) {
            const Individuo& Padre1 = Poblacion[i];

            // Padre 2: otro individuo de la población al azar, distinto del padre 1.
            int j = distPareja(rng);
            if (j >= i) j++;
            const Individuo& Padre2 = Poblacion[j];

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

        cout << "Generacion " << gen << ": mejor fitness " << Poblacion[0].fitness << endl;
    }

    return 0;
}
