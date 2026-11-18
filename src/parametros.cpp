#include "parametros.h"

#include <iostream>
#include <random>
#include <string>

using namespace std;

namespace {

// Valores que toma cada parámetro cuando no se entrega por línea de comandos.
// La semilla no tiene valor por defecto: si no se entrega, se sortea en leerParametros.
Parametros parametrosPorDefecto() {
    Parametros p;

    // Ejecución
    p.semilla = 0;
    p.poblacion = 500;
    p.nGeneraciones = 10000;
    p.genSinMejora = 250;

    // Restricciones y evaluación
    p.maxTrips = 3;
    p.lambdaCap = 1;
    p.lambdaT = 1;

    // Inicialización
    p.rcl = 0.6;

    // Cruce
    p.probCruzamiento = 0.6;

    // Mutaciones
    p.probMutDeposito = 0.2;
    p.probMutClientes = 0.2;
    p.probMutIntercambio = 0.2;

    // Espacio de creencias normativo: tabla de cierres
    p.cierresFraccion = 0.2;
    p.cierresOlvido = 0.7;
    p.cierresPiso = 0.1;
    p.cierresGenSesgo = 20;

    // Espacio de creencias circunstancial: archivo de mejores
    p.mejoresFraccion = 0.2;
    p.mejoresTam = 50;
    p.mejoresProb = 0.2;

    return p;
}

} // namespace

Parametros leerParametros(int argc, char* argv[]) {
    Parametros p = parametrosPorDefecto();
    bool semillaEntregada = false;

    // argv[1] es la instancia; desde argv[2] vienen pares "--nombre valor".
    for (int i = 2; i + 1 < argc; i += 2) {
        string bandera = argv[i];
        string valor = argv[i + 1];

        // Ejecución
        if (bandera == "--semilla") {
            p.semilla = (unsigned)stoul(valor);
            semillaEntregada = true;
        }
        else if (bandera == "--poblacion")          p.poblacion = stoi(valor);
        else if (bandera == "--nGeneraciones")      p.nGeneraciones = stoi(valor);
        else if (bandera == "--genSinMejora")       p.genSinMejora = stoi(valor);
        // Restricciones y evaluación
        else if (bandera == "--maxTrips")           p.maxTrips = stoi(valor);
        else if (bandera == "--lambdaCap")          p.lambdaCap = stod(valor);
        else if (bandera == "--lambdaT")            p.lambdaT = stod(valor);
        // Inicialización
        else if (bandera == "--rcl")                p.rcl = stod(valor);
        // Cruce
        else if (bandera == "--probCruzamiento")    p.probCruzamiento = stod(valor);
        // Mutaciones
        else if (bandera == "--probMutDeposito")    p.probMutDeposito = stod(valor);
        else if (bandera == "--probMutClientes")    p.probMutClientes = stod(valor);
        else if (bandera == "--probMutIntercambio") p.probMutIntercambio = stod(valor);
        // Espacio de creencias normativo: tabla de cierres
        else if (bandera == "--cierresFraccion")    p.cierresFraccion = stod(valor);
        else if (bandera == "--cierresOlvido")      p.cierresOlvido = stod(valor);
        else if (bandera == "--cierresPiso")        p.cierresPiso = stod(valor);
        else if (bandera == "--cierresGenSesgo")    p.cierresGenSesgo = stoi(valor);
        // Espacio de creencias circunstancial: archivo de mejores
        else if (bandera == "--mejoresFraccion")    p.mejoresFraccion = stod(valor);
        else if (bandera == "--mejoresTam")         p.mejoresTam = stoi(valor);
        else if (bandera == "--mejoresProb")        p.mejoresProb = stod(valor);
    }

    // Sin --semilla se sortea una; queda en p.semilla para poder imprimirla y repetir la ejecución.
    if (!semillaEntregada) {
        random_device rd;
        p.semilla = rd();
    }
    return p;
}

void imprimirUso(const char* programa) {
    Parametros d = parametrosPorDefecto();
    cout << "Uso: " << programa << " <instancia.dat> [--nombre valor] ...\n\n"
         << "Opciones (entre parentesis, el valor por defecto):\n"
         << "  Ejecucion\n"
         << "    --semilla N             Semilla del generador aleatorio (se sortea si no se entrega)\n"
         << "    --poblacion N           Tamano de la poblacion (" << d.poblacion << ")\n"
         << "    --nGeneraciones N       Maximo de generaciones (" << d.nGeneraciones << ")\n"
         << "    --genSinMejora N        Termina tras N generaciones sin mejora; 0 = no corta (" << d.genSinMejora << ")\n"
         << "  Restricciones y evaluacion\n"
         << "    --maxTrips N            Maximo de trips por route; 0 = sin limite (" << d.maxTrips << ")\n"
         << "    --lambdaCap X           Castigo por unidad de exceso de carga (" << d.lambdaCap << ")\n"
         << "    --lambdaT X             Castigo por unidad de exceso de tiempo (" << d.lambdaT << ")\n"
         << "  Inicializacion\n"
         << "    --rcl X                 Fraccion de candidatos mas cercanos del greedy (" << d.rcl << ")\n"
         << "  Cruce\n"
         << "    --probCruzamiento X     Probabilidad de cruzar (" << d.probCruzamiento << ")\n"
         << "  Mutaciones\n"
         << "    --probMutDeposito X     Probabilidad de mutar cada deposito (" << d.probMutDeposito << ")\n"
         << "    --probMutClientes X     Probabilidad, por vehiculo, de barajar clientes en sus trips (" << d.probMutClientes << ")\n"
         << "    --probMutIntercambio X  Probabilidad, por vehiculo, de intercambiar dos trips (" << d.probMutIntercambio << ")\n"
         << "  Espacio de creencias normativo: tabla de cierres\n"
         << "    --cierresFraccion X     Fraccion mejor de la poblacion que alimenta la tabla (" << d.cierresFraccion << ")\n"
         << "    --cierresOlvido X       Factor de olvido de la tabla por generacion (" << d.cierresOlvido << ")\n"
         << "    --cierresPiso X         Peso minimo de cada deposito al sortear un retorno (" << d.cierresPiso << ")\n"
         << "    --cierresGenSesgo N     Generacion desde la que se sortea con la tabla (" << d.cierresGenSesgo << ")\n"
         << "  Espacio de creencias circunstancial: archivo de mejores\n"
         << "    --mejoresFraccion X     Fraccion mejor de la poblacion que postula al archivo (" << d.mejoresFraccion << ")\n"
         << "    --mejoresTam N          Tamano maximo del archivo (" << d.mejoresTam << ")\n"
         << "    --mejoresProb X         Probabilidad de que el padre 2 salga del archivo (" << d.mejoresProb << ")\n\n"
         << "Ejemplo:\n"
         << "  " << programa << " instancia.dat --semilla 7 --poblacion 200 --lambdaT 100\n";
}

void imprimirParametros(const Parametros& p) {
    cout << " --------------- Parametros ---------------" << endl;
    cout << "semilla: " << p.semilla
         << "  poblacion: " << p.poblacion
         << "  nGeneraciones: " << p.nGeneraciones
         << "  genSinMejora: " << p.genSinMejora << endl;
    cout << "maxTrips: " << p.maxTrips
         << "  lambdaCap: " << p.lambdaCap
         << "  lambdaT: " << p.lambdaT
         << "  rcl: " << p.rcl << endl;
    cout << "probCruzamiento: " << p.probCruzamiento
         << "  probMutDeposito: " << p.probMutDeposito
         << "  probMutClientes: " << p.probMutClientes
         << "  probMutIntercambio: " << p.probMutIntercambio << endl;
    cout << "cierresFraccion: " << p.cierresFraccion
         << "  cierresOlvido: " << p.cierresOlvido
         << "  cierresPiso: " << p.cierresPiso
         << "  cierresGenSesgo: " << p.cierresGenSesgo << endl;
    cout << "mejoresFraccion: " << p.mejoresFraccion
         << "  mejoresTam: " << p.mejoresTam
         << "  mejoresProb: " << p.mejoresProb << endl;
}
