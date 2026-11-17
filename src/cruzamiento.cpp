#include "cruzamiento.h"

#include <algorithm>
#include <iostream>
#include <random>

using namespace std;

Individuo iniciarCruzamiento(const Individuo& Padre1, const Individuo& Padre2) {
    cout << "Implementacion" << endl;

}

Individuo cruzarTrips(const Solucion& Padre1, const Solucion& Padre2, const Instancia& inst, const Parametros& p, mt19937& rng) {
    Solucion hijo = Padre2; // El hijo parte como una copia del padre 2

    for (int k = 0; k < inst.numVehicles; k++){ // Por CADA VEHICULO intentaremos insertar un trip.
        const vector<Trip>& tripsP1 = Padre1[k].trips;
        if (tripsP1.empty()) continue;
        uniform_int_distribution<int> distTrip(0, tripsP1.size()-1);
        const Trip& tripDeInsercion = tripsP1[distTrip(rng)]; // Se elige un trip de insercion del padre 1 para insertar en el hijo

        // Se trabaja sobre una copia pues si no hay cupo en terminos de trips (por maxTrips) se descarta y el hijo queda intacto
        Solucion tentativo = hijo;

        // Se busca borrar del tentativo los clientes del trip a insertar, estén donde estan para evitar duplicados y posteriores reparaciones.
        // Luego, en el caso que aplique se eliminan los trips que quedaron vacíos.
        for (Route& route: tentativo) {
            for (Trip& trip: route.trips) {
                auto& clients = trip.clientes;
                clients.erase(remove_if(clients.begin(), clients.end(), [&](int c){
                    return find(tripDeInsercion.clientes.begin(), tripDeInsercion.clientes.end(), c) != tripDeInsercion.clientes.end();
                }), clients.end()); // Borra sobre el hijo, en el caso que aplique, los clientes que se donarán por parte del Padre 1.
            }
            auto& tr= route.trips;
            tr.erase(remove_if(tr.begin(),tr.end(), [](const Trip& t){return t.clientes.empty();}), tr.end()); // En el caso de que haya
        } // LA LIMPIEZA PARA EVITAR DUPLICADOS FINALIZA

        // Insertar en una posicion al azar el trip elegido proveniente de Padre 1, solo si la route tiene cupo (MaxTrips se debe respetar).
        vector<Trip>& destino = tentativo[k].trips;
        if ( p.maxTrips > 0 && (int)destino.size() >= p.maxTrips) continue; // Significa que si no hay cupo se debe descartar el tentativo, no se inserta para esta ronda, para respetar MaxTrips.
        uniform_int_distribution<int> distPos(0,destino.size());
        destino.insert(destino.begin() + distPos(rng), tripDeInsercion);

        hijo = tentativo;
    }

    // Checkeo final, el último trip de cada ruta (remember ruta === vehiculo abstractamente) debe volver a un depósito donde el vehiculo
    // puedeRecargar y puedeTerminar. Si no, se reasigna al permitido más cercano.
    for (int k = 0; k < inst.numVehicles; k++) corregirCierre(inst, k, hijo[k]);

    Individuo ind;
    ind.sol = hijo;
    ind.eval = evaluar(inst, ind.sol);
    ind.fitness = fitness(ind.eval, p.lambdaCap, p.lambdaT);
    return ind;
}
