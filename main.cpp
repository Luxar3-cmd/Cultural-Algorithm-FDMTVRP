#include <algorithm>
#include <iostream>
#include <random>
#include <vector>

#include "instancia.h"
#include "solucion.h"

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
    // ---------------------------------------------------------
    Parametros p;
    p.lambdaCap = 1;
    p.lambdaT = 1;
    p.maxTrips = 3;
    p.poblacion = 1000;
    p.rcl = 0.6;
    p.PorcentajeMejor = 0.2;

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
    //    Archivo de mejores individuos: entra el mejor de cada generación.
    // ---------------------------------------------------------
    vector<Individuo> Mejores;
    Mejores.push_back(Poblacion[0]);

    // ---------------------------------------------------------
    // 6. Espacio de creencias normativo
    //    TablaCierres[cliente][deposito]: cuántas veces un trip que termina en ese cliente
    //    volvió a ese depósito, contado por trip sobre el X% mejor de la población.
    //    DISCLAIMER: una fila por nodo para indexar directo con el índice interno; las
    //    primeras n filas (con n la cantidad de depósitos) quedan sin uso a propósito.
    // ---------------------------------------------------------
    vector<vector<double>> TablaCierres(inst.idxToId.size(), vector<double>(inst.depots.size(), 0));

    int cantidadInspeccionados = (int)(Poblacion.size() * p.PorcentajeMejor);
    for (int i = 0; i < cantidadInspeccionados; i++) {
        for (const Route& route : Poblacion[i].sol) {
            for (const Trip& trip : route.trips) {
                TablaCierres[trip.clientes.back()][trip.retorno] += 1;
            }
        }
    }

    return 0;
}
