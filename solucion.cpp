#include "solucion.h"

#include <algorithm>
#include <iostream>
#include <random>
#include <algorithm>

using namespace std;

// Se declara en namespace para que esta funcion no sea accesible dentro de otros archivos cpp.
namespace {
// Tiempo del arco i -> j. Aborta si el arco no existe en la instancia.
int arco(const Instancia& inst, int i, int j) {
    int t = inst.time[i][j];
    if (t == INF) {
        cerr << "Arco inexistente en la solucion: " << inst.idxToId[i] << " -> " << inst.idxToId[j] << endl;
        exit(1);
    }
    return t;
}

} // namespace

Evaluacion evaluar(const Instancia& inst, const Solucion& sol) {
    Evaluacion e{0, 0, 0}; // Inicialización del objeto de evaluación.

    // Por cada vehículo se consulta la ruta que este siguió.
    for (const Route& route : sol) {
        int tiempoRoute = 0;
        int nodo_actual = route.inicio; // Empieza por el depósito de inicio señalado por la representación.

        for (const Trip& trip : route.trips) {
            int carga = 0;
            for (int c : trip.clientes) {
                tiempoRoute += arco(inst, nodo_actual, c);
                carga += inst.demand[c];
                nodo_actual = c;
            }
            tiempoRoute += arco(inst, nodo_actual, trip.retorno);
            nodo_actual = trip.retorno;   // el siguiente trip parte de aquí, sin costo

            e.excesoCarga += max(0, carga - inst.cap);
        }

        e.tiempo += tiempoRoute;
        e.excesoTiempo += max(0, tiempoRoute - inst.tMax);
    }
    return e;
}

double fitness(const Evaluacion& e, double lambdaCap, double lambdaT) {
    return e.tiempo + lambdaCap * e.excesoCarga + lambdaT * e.excesoTiempo;
}

Solucion inicializarSolucion(const Instancia& inst, const Parametros& p, std::mt19937& rng) {
    Solucion sol(inst.numVehicles); // Cada vehículo tendrá su Route. Pueden estar vacias.
    vector<int> clientesDisponibles = inst.clientsC;

    // Tamaño de la RCL: rcl · |C| como en el paper, acotado a [1, candidatos].
    const int tamRcl = max(1, (int)(p.rcl * inst.clientsC.size()));

    // Se construye la route completa de cada vehículo (modo secuencial).
    for (int i = 0; i < inst.numVehicles; i++) {
        const Vehiculo& veh = inst.vehiculos[i];
        Route& route = sol[i];

        // ---- Depósito inicial: sorteo uniforme entre los permitidos ----
        vector<int> permitidos;
        for (int d : inst.depots)
            if (veh.puedeSalir[d]) permitidos.push_back(d);
        if (permitidos.empty()) {
            cerr << "Vehiculo " << i + 1 << " no tiene deposito de salida permitido" << endl;
            exit(1);
        }
        uniform_int_distribution<int> distDepot(0, permitidos.size() - 1);
        route.inicio = permitidos[distDepot(rng)]; // Aquí ocurre el sorteo del permitido.
        int actual = route.inicio;

        // ---- Trips: uno por vuelta, hasta maxTrips (0 = sin límite) o sin clientes ----
        while ((p.maxTrips <= 0 || (int)route.trips.size() < p.maxTrips) && !clientesDisponibles.empty()) {
            Trip trip;
            int carga = 0;

            // ---- Clientes: uno por vuelta, mientras alguno quepa en cap − carga ----
            while (true) {
                vector<int> caben;
                for (int c : clientesDisponibles)
                    if (inst.demand[c] <= inst.cap - carga) caben.push_back(c);
                if (caben.empty()) break;

                // Más cercanos primero desde el nodo actual; la RCL son los primeros k.
                sort(caben.begin(), caben.end(),
                     [&](int a, int b) { return inst.time[actual][a] < inst.time[actual][b]; });
                int k = min(tamRcl, (int)caben.size());
                uniform_int_distribution<int> distRcl(0, k - 1);
                int elegido = caben[distRcl(rng)]; // Aquí influye el RCL

                trip.clientes.push_back(elegido);
                clientesDisponibles.erase(find(clientesDisponibles.begin(), clientesDisponibles.end(), elegido));
                carga += inst.demand[elegido];
                actual = elegido;
            }
            if (trip.clientes.empty()) break; // ningún cliente cabe ni en un trip vacío (demanda > cap)

            // ---- Retorno: depósito compatible más cercano. El último trip además debe poder terminar ----
            bool ultimo = clientesDisponibles.empty() || (p.maxTrips > 0 && (int)route.trips.size() + 1 >= p.maxTrips);
            int retorno = -1;
            /**
             * Revisar cada depósito y quedarse con el mejor.
             */
            for (int d : inst.depots) {
                if (!veh.puedeRecargar[d]) continue;
                if (ultimo && !veh.puedeTerminar[d]) continue;
                if (retorno == -1 || inst.time[actual][d] < inst.time[actual][retorno]) retorno = d;
            }
            if (retorno == -1) {
                cerr << "Vehiculo " << i + 1 << ", trip " << route.trips.size() + 1
                     << ": no hay deposito de retorno compatible" << endl;
                exit(1);
            }
            trip.retorno = retorno;
            actual = retorno;
            route.trips.push_back(trip);
        }
    }

    // ---- Sin cupo: los clientes restantes van al último trip construido, con exceso de carga ----
    if (!clientesDisponibles.empty()) {
        // Quedan clientes que sumar
        Trip* ultimoTrip = nullptr; // Se declara un puntero para luego agregar el resto de clientes a esa Route.
        for (Route& route : sol)
            if (!route.trips.empty()) ultimoTrip = &route.trips.back();
        if (ultimoTrip == nullptr) { // Caso borde muy borde que no deberia pasar.
            cerr << "No se construyo ningun trip; quedan " << clientesDisponibles.size() << " clientes sin servir" << endl;
            exit(1);
        }
        for (int c : clientesDisponibles) ultimoTrip->clientes.push_back(c);
    }

    return sol;
}

void imprimirSolucion(const Instancia& inst, const Solucion& sol) {

    cout << " ---------------------- solución obtenida ---------------------- " << endl;
    for (size_t k = 0; k < sol.size(); k++) {
        const Route& route = sol[k];
        cout << "vehiculo " << k + 1 << ": ";
        if (route.trips.empty()) { cout << "(sin uso)" << endl; continue; }
        cout << inst.idxToId[route.inicio];
        for (const Trip& trip : route.trips) {
            for (int c : trip.clientes) cout << " -> " << inst.idxToId[c];
            cout << " -> [" << inst.idxToId[trip.retorno] << "]";
        }
        cout << endl;
    }
    Evaluacion e = evaluar(inst, sol);
    cout << "tiempo: " << e.tiempo
         << "  excesoCarga: " << e.excesoCarga
         << "  excesoTiempo: " << e.excesoTiempo
         << "  factible: " << (e.factible() ? "si" : "no") << endl;
    cout << " ---------------------- Final de la solución ----------------------" << endl;
}
