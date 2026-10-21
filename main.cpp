#include <iostream>

#include "instancia.h"
#include "solucion.h"

using namespace std;

int main(int argc, char* argv[]) {

    // TODO: Hacer una infraestructura basada en objetos OOP para la utilización de algoritmo cultural

    // Leida y captura de la instancia.
    if (argc < 2) { cerr << "Uso: " << argv[0] << " <archivo.dat>" << endl; return 1;}
    Instancia inst;
    inicializarInstancia(inst, argv[1]);
    imprimirResumen(inst);

    // -------
    Parametros p;
    p.lambdaCap = 1;
    p.lambdaT = 1;
    p.maxTrips = 3;
    p.poblacion = 1;
    p.rcl = 0.4;
    mt19937 rng(p.semilla);


    Solucion solucion = inicializarSolucion(inst, p, rng);
    imprimirSolucion(inst, solucion);

    return 0;
}
