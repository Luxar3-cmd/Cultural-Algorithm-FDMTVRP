#include "creencias.h"
#include <algorithm>

using namespace std;

void actualizarTablaCierres(vector<vector<double>>& tablaCierres, const vector<Individuo>& poblacion, const Parametros& p) {
    // Olvido gradual: lo aprendido en generaciones anteriores pierde peso
    for (vector<double>& fila: tablaCierres) {
        for (double&casilla: fila) casilla *= p.cierresOlvido;
    }

    // Aprendizaje: Se cuenta por trip sobre la p.cierresFraccion mejor de la población
    int cantidadInspeccionados = (int)(poblacion.size() * p.cierresFraccion);
    for (int i = 0; i < cantidadInspeccionados; i++) { // Para cada individuo
        for (const Route& route: poblacion[i].sol) {
            for (const Trip& trip: route.trips) {
                tablaCierres[trip.clientes.back()][trip.retorno] += 1;
            }
        }
    }
}

void actualizarMejores(vector<Individuo>& mejores, const vector<Individuo>& poblacion, const Parametros& p) {
    // Candidatos: la p.mejoresFraccion mejor de la poblacion entra al "salón de la fama".
    int cantidadCandidatos = (int)(poblacion.size() * p.mejoresFraccion);
    mejores.insert(mejores.end(), poblacion.begin(), poblacion.begin() + cantidadCandidatos);

    // Se ordena por fitness.
    sort(mejores.begin(), mejores.end(),
        [](const Individuo& a, const Individuo& b){ return a.fitness <  b.fitness;});

    // Sin duplicados: de cada grupo con el mismo fitness queda uno solo.
    mejores.erase(unique(mejores.begin(), mejores.end(),
        [](const Individuo& a, const Individuo& b) { return a.fitness == b.fitness; }),
        mejores.end());

    if ((int)mejores.size() > p.mejoresTam) mejores.resize(p.mejoresTam);
}
