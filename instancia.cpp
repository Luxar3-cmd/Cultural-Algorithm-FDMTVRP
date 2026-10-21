#include "instancia.h"

#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

// Funciones auxiliares del lector; no forman parte de la interfaz pública.
namespace {

/**
 * leerArchivo, esta función recibe el nombre del archivo a leer y utiliza la librería
 * stringstream para entregar un puntero que apunte al archivo y lo convierte en string
 * en su totalidad.
 * */
string leerArchivo(const string& filename) {
    ifstream archivo_instancia(filename);
    if (!archivo_instancia.is_open()) {
        cerr << "No se pudo abrir " << filename << endl;
        exit(1);
    }
    stringstream ss;
    ss << archivo_instancia.rdbuf(); // Copiar al objeto ss la totalidad del archivo leido
    return ss.str();
}

/**
 * Función que se dedica a quitar los comentarios que las intancias tienen,
 * facilita procesarlas posteriormente.
 * */
void quitarComentarios(string& texto) {
    size_t inicio;
    while ((inicio = texto.find("/*")) != string::npos) {
        size_t cierre = texto.find("*/", inicio);
        if (cierre == string::npos) {
            cerr << "Comentario sin cerrar" << endl;
            exit(1);
        }
        texto.erase(inicio, cierre + 2 - inicio);
    }
}

/**
 * Función que "tokeniza" el texto obtenido del archivo de instancias
 * recorre cada cadena de texto, utilizando el espacio en blanco como separador,
 * así se obtiene un vector el cual en cada posición se encuentra una cadena de texto
 * lista para procesar.
 * */
vector<string> tokenizar(const string& texto) {
    vector<string> tokens;
    istringstream iss(texto);
    for (string tok; iss >> tok;)
        tokens.push_back(tok);
    return tokens;
}

} // namespace

void inicializarInstancia(Instancia& inst, const string& filename) {
    string texto = leerArchivo(filename); // Pasa el archivo a string
    quitarComentarios(texto); // Quita los comentarios del string
    vector<string> tokens = tokenizar(texto); // se convierten en tokens.

    // Inicialización de la instancia
    for (auto it = tokens.begin(); it != tokens.end(); ++it) {
        // 1. Depósitos de inicio: crean índice interno y entran a depots.
        if (*it == "D_start:=") {
            ++it;
            while (*it != ";") {
                int id = stoi(*it); // Identificador de la instancia
                int idx = inst.idxToId.size(); // Identificador en esta implementación
                inst.idToIdx[id] = idx; // Ejemplo : 377 -> 0
                inst.idxToId.push_back(id); // Ejemplo : idxToId[0] = 377
                inst.depots.push_back(idx); // depots = {0}
                ++it;
            }
        // 2. Clientes: crean índice interno y entran a clientsC.
        } else if (*it == "C:=") {
            ++it;
            while (*it != ";") {
                int id = stoi(*it);
                int idx = inst.idxToId.size();
                inst.idToIdx[id] = idx;
                inst.idxToId.push_back(id);
                inst.clientsC.push_back(idx);
                ++it;
            }
        // 3. Depósitos intermedios y finales: solo alias al depósito base (id - 10000 / id - 100000).
        } else if (*it == "D_in:=") {
            ++it;
            while (*it != ";") {
                int id = stoi(*it);
                int base = id - 10000; // Para referenciar al depósito base.
                inst.idToIdx[id] = inst.idToIdx.at(base); // Referencia al mismo id de depósito.
                ++it;
            }
        } else if (*it == "D_end:=") {
            ++it;
            while (*it != ";") {
                int id = stoi(*it);
                int base = id - 100000;
                inst.idToIdx[id] = inst.idToIdx.at(base);
                ++it;
            }
        // 4. Vehículos: se crean con permisos en 0; los unos los ponen ava_start / com / ava_end.
        } else if (*it == "K:=") {
            ++it;
            while (*it != ";") {
                Vehiculo k;
                k.puedeSalir = vector<char>(inst.depots.size(),0);
                k.puedeRecargar = vector<char>(inst.depots.size(),0);
                k.puedeTerminar = vector<char>(inst.depots.size(),0);
                inst.vehiculos.push_back(k);
                ++it;
            }
            inst.numVehicles = inst.vehiculos.size();
        } else if (*it == "V:=") {
            ++it;
            int maxTrips = 0;
            while (*it != ";") {
                maxTrips++;
                ++it;
            }
            inst.maxTrips = maxTrips;
        } else if (*it == "tmax:=") {
            ++it;
            inst.tMax = stoi(*it);
            ++it;
        } else if (*it == "cap:=") {
            ++it;
            inst.cap = stoi(*it);
            ++it;
        // 5. Matriz de tiempos: n x n en INF, se escriben solo los arcos listados.
        } else if (*it == "time_:=") {
            int n = inst.idxToId.size();
            inst.time.assign(n, vector<int>(n, INF));
            ++it;
            while (*it != ";") {
                int i = inst.idToIdx.at(stoi(*it)); ++it;
                int j = inst.idToIdx.at(stoi(*it)); ++it;
                int cost = stoi(*it);                ++it;
                inst.time[i][j] = cost;
            }
        // 6. Demanda por nodo (los depósitos vienen con 0).
        } else if (*it == "dem:=") {
            inst.demand.assign(inst.idxToId.size(), 0);
            ++it;
            while (*it != ";") {
                int idx = inst.idToIdx.at(stoi(*it)); ++it;
                inst.demand[idx] = stoi(*it);         ++it;
            }
        // 7. Permisos: triples (depósito, vehículo, flag). Lo no listado queda en 0.
        } else if (*it == "ava_start:=" || *it == "com:=" || *it == "ava_end:=") {
            string bloque = *it;
            ++it;
            while (*it != ";") {
                int d = inst.idToIdx.at(stoi(*it)); ++it; // depósito
                int k = stoi(*it) - 1;              ++it; // vehiculo
                char flag = stoi(*it);              ++it; // Permiso binario
                Vehiculo& v = inst.vehiculos.at(k);
                if (bloque == "ava_start:=")    v.puedeSalir[d] = flag;
                else if (bloque == "com:=")     v.puedeRecargar[d] = flag;
                else                            v.puedeTerminar[d] = flag;
            }
        }
    }
}

void imprimirResumen(const Instancia& inst) {
    cout << " --------------- Resumen de la instancia ---------------" << endl;

    cout << "nodos: " << inst.idxToId.size()
         << "  depots: " << inst.depots.size()
         << "  clientes: " << inst.clientsC.size()
         << "  vehiculos: " << inst.numVehicles
         << "  maxTrips: " << inst.maxTrips
         << "  tMax: " << inst.tMax
         << "  cap: " << inst.cap << endl;

    int arcos = 0;
    for (const auto& fila : inst.time)
        for (int t : fila) if (t != INF) arcos++;
    cout << "arcos cargados: " << arcos << endl;

    for (int d : inst.depots)
        cout << "deposito idx " << d << " = id " << inst.idxToId[d] << endl;

    for (int k = 0; k < inst.numVehicles; k++) {
        const Vehiculo& v = inst.vehiculos[k];
        cout << "vehiculo " << k + 1 << "  salir:";
        for (int d : inst.depots) cout << ' ' << (int)v.puedeSalir[d];
        cout << "  recargar:";
        for (int d : inst.depots) cout << ' ' << (int)v.puedeRecargar[d];
        cout << "  terminar:";
        for (int d : inst.depots) cout << ' ' << (int)v.puedeTerminar[d];
        cout << endl;
    }
}
