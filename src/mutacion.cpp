#include "mutacion.h"

#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>
#include <utility>

using namespace std;

Solucion mutarDeposito(const Instancia& inst, const Parametros& p, Solucion sol, const vector<vector<double>>& tablaCierres, int generacion, mt19937& rng) {
    bernoulli_distribution moneda(p.probMutDeposito);
    bool sesgar = generacion >= p.cierresGenSesgo; // Antes de esa generacion el sorteo sobre los depósitos es uniforme

    for ( int k = 0; k < inst.numVehicles; k++) { // POR CADA VEHICULO (ROUTE) VEREMOS LOS DEPÓSITO UNO A UNO. (INICIO Y RETORNOS)
        Route& route = sol[k];
        if (route.trips.empty()) continue; // Vehiculo (Route) sin uso, nada que hacer
        const Vehiculo& veh = inst.vehiculos[k];

        // --- Depósito de inicio: Finalidad exploratoria, se elige al azar entre los permitidos distintos del actual ---
        if (moneda(rng)) {
            vector<int> candidatos;
            for (int d: inst.depots) {
                if (veh.puedeSalir[d] && d != route.inicio) candidatos.push_back(d);
            }
            if (!candidatos.empty()) {
                uniform_int_distribution<int> dist(0, candidatos.size() - 1);
                route.inicio = candidatos[dist(rng)]; // Se reemplaza la ruta de inicio por un candidato válido
            }
        }

        // --- Depósitos de retorno por cada trip: guiados por la tabla de cierres ---
        for (size_t t = 0; t < route.trips.size(); t++) {
            if (!moneda(rng)) continue;
            Trip& trip = route.trips[t];
            bool ultimo = (t + 1 == route.trips.size()); // Flag necesaria para validar que el depósito de retorno ademas tiene los permisos para terminar la ruta del vehiculo en cuestión.
            int ultimoCliente = trip.clientes.back();

            // Candidatos factibles según la posicion del trip, con su peso guiado por el espacio de creencias normativo.
            vector<int> candidatos;
            vector<double> pesos; // Cuanta relevancia tiene cada candidato, relacionado con el espacio de creencias y el mecanismo de influencia.
            for ( int d: inst.depots) {
                if (d == trip.retorno) continue; // Evita agregar a los candidatos el mismo depósito de retorno
                if (!veh.puedeRecargar[d]) continue; // Evita agregar a los depósitos del cual el vehiculo no tiene permiso para recargar.
                if (ultimo && !veh.puedeTerminar[d]) continue; // Eviata agregar a la lista de candidatos depósitos que no tienen el permiso para terminar el trip, en el caso de que sea el último trip.
                candidatos.push_back(d);
                pesos.push_back(sesgar ? tablaCierres[ultimoCliente][d] + p.cierresPiso : 1.0); // En el caso de que ya este permitido aplicar el espacio de creencias normativo, se guarda en el vector de pesos ese valor.
            }
            if (candidatos.empty()) continue; // No hay depósito factible para aplicar mutacion en el trip seleccionado

            discrete_distribution<int> dist(pesos.begin(), pesos.end());
            trip.retorno = candidatos[dist(rng)];
        }
    }
    return sol;
}

Solucion mutarClientes (const Instancia& inst, const Parametros& p, Solucion sol, mt19937& rng) {
    bernoulli_distribution moneda(p.probMutClientes); // Se tira una moneda cargada con la probabilidad de mutacion configurada.

    for (int k = 0; k < inst.numVehicles; k++) {
        vector<Trip>& trips = sol[k].trips; // Por cada vehículo accedemos a sus trips.
        if (trips.empty()) continue;
        if (!moneda(rng)) continue; // Hay una chance de que el vehiculo salga seleccionado para mutar.

        // La cantidad de trips para ese vehiculo dentro de la que se van a baarajar clientes.
        int maximo = (int) ceil(trips.size() / 2.0); //
        uniform_int_distribution<int> distCantidad(1, maximo);
        int cantidad = distCantidad(rng);

        // Se eligen cuáles trips se van a barajar.
        vector<int> posiciones(trips.size()); // ej [0,0,0,0]
        iota(posiciones.begin(), posiciones.end(),0); // ej [0,1,2,3]
        shuffle(posiciones.begin(),posiciones.end(), rng);

        for (int i = 0; i < cantidad; i++) {
            vector<int>& clientes = trips[posiciones[i]].clientes;
            shuffle(clientes.begin(), clientes.end(), rng);
        }
    }
    return sol;
}

Solucion intercambiarTrips( const Instancia& inst, const Parametros& p, Solucion sol, mt19937& rng) {
    bernoulli_distribution moneda(p.probMutIntercambio); // Una moneda cargada por vehiculo

    for (int k = 0; k < inst.numVehicles; k++) {
        vector<Trip>& trips = sol[k].trips;
        int n = trips.size();
        if (n < 2) continue; // NO hay dos trips que intercambiar
        if (!moneda(rng)) continue;

        // Se sortean dos posiciones distintas: i entre todas, j entre las n-1 restantes.
        uniform_int_distribution<int> distI(0, n-1);
        uniform_int_distribution<int> distJ(0, n-2);
        int i = distI(rng);
        int j = distJ(rng);
        if ( j >= i) j++; // Salta la posicion i

        swap(trips[i],trips[j]);
        corregirCierre(inst, k, sol[k]);  // Checkea el caso de que el nuevo último trip no pueda terminar por permisos.
    }
    return sol;
}
