#include <iostream>

#include "instancia.h"

using namespace std;

struct Trip {vector<int> clientes; int retorno;};
struct Route {int inicio; vector<Trip> trips;};

int main(int argc, char* argv[]) {

    // TODO: Hacer una infraestructura basada en objetos para la utilización de algoritmo cultural

    // Leida y captura de la instancia.
    if (argc < 2) { cerr << "Uso: " << argv[0] << " <archivo.dat>" << endl; return 1;}
    Instancia inst;
    inicializarInstancia(inst, argv[1]);
    imprimirResumen(inst);

    return 0;
}
