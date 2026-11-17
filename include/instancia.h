#ifndef INSTANCIA_H
#define INSTANCIA_H

#include <limits>
#include <string>
#include <unordered_map>
#include <vector>

// Arco inexistente en la matriz de tiempos.
constexpr int INF = std::numeric_limits<int>::max() * 2e-1;

/**
 * Estructura declarada por vehículo, la idea detrás de esta infra es que
 * el objeto me diga qué tiene permitido hacer para cada depósito.
 * Cada vector se indexa por índice interno de depósito; 1 = permitido.
 * */
struct Vehiculo {
    std::vector<char> puedeSalir;
    std::vector<char> puedeRecargar;
    std::vector<char> puedeTerminar;
};

struct Instancia {
    std::vector<int> depots, clientsC;

    // Índice interno para referenciar los id de instancias a los identificadores del código.
    std::unordered_map<int,int> idToIdx;
    // Índice interno para referenciar en base al los identificadores del código al id de la instancia.
    std::vector<int> idxToId;
    // Demanda de cada nodo (considera depositos y clientes)
    std::vector<int> demand;

    // tMax: Cantidad de tiempo máximo de "jornada laboral"
    // maxTrips: Numero máximo de trips en cada route.
    // numVehicles: Cantidad de vehículos disponibles.
    // cap: Capacidad de los vehículos de reparto.
    int tMax, maxTrips, numVehicles, cap;

    // Informa sobre la disponibilidad que un vehículo tiene con respecto a cada depósito
    std::vector<Vehiculo> vehiculos;

    // Matriz de costos
    std::vector<std::vector<int>> time;  //time[i][j], INF cuando arco no existe.
};

// Lee un archivo .dat de instancia y rellena inst. Aborta con mensaje si el archivo
// no se puede abrir o si un identificador no corresponde a ningún nodo declarado.
void inicializarInstancia(Instancia& inst, const std::string& filename);

// Imprime un resumen de la instancia leída, para verificar la carga.
void imprimirResumen(const Instancia& inst);

#endif // INSTANCIA_H
