
# Implementación de algoritmo cultural

## Compilación y ejecución

Requiere Linux, `g++` con C++17 y `make`.

```
make          # compila y deja el ejecutable ./main
make clean    # borra el ejecutable y la carpeta build/
```

El programa recibe la instancia como primer argumento y, después, los parámetros que se quieran cambiar:

```
./main <instancia.dat> [--nombre valor] ...
```

Ejemplos, con las instancias descomprimidas en `Instancias/`:

```
./main Instancias/2-InstanciasB_redefinidas/b-Instancia11_veh2_cap2_rel1.dat
./main Instancias/2-InstanciasB_redefinidas/b-Instancia11_veh2_cap2_rel1.dat --semilla 7 --poblacion 200
./main Instancias/5-InstanciasE_redefinidas/e-Instancia41_veh3_cap15_rel1.dat --lambdaT 100 --nGeneraciones 500
```

`./main --help` muestra todas las opciones.

`make run <instancia.dat>` compila si hace falta y ejecuta en un solo paso. Para pasar parámetros por esa vía hay que anteponer `--`, o `make` los toma como opciones propias. Los dos comandos siguientes especifican todos los parámetros, con los valores que entregó la sintonización con irace para cada set (ver `sintonizacion/`).

Set B (10 clientes). `--nGeneraciones 96` con `--poblacion 514` equivale al presupuesto de 50.000 evaluaciones usado al sintonizar:

```
make run Instancias/2-InstanciasB_redefinidas/b-Instancia14_veh2_cap2_rel1.dat -- \
    --semilla 7 \
    --poblacion 514 \
    --nGeneraciones 96 \
    --maxTrips 3 \
    --lambdaCap 186.6979 \
    --lambdaT 10.6254 \
    --rcl 0.5912 \
    --probCruzamiento 0.971 \
    --probMutDeposito 0.156 \
    --probMutClientes 0.3067 \
    --probMutIntercambio 0.066 \
    --cierresFraccion 0.2609 \
    --cierresOlvido 0.5075 \
    --cierresPiso 0.027 \
    --cierresGenSesgo 237 \
    --mejoresFraccion 0.2235 \
    --mejoresTam 179 \
    --mejoresProb 0.4955
```

Set E (100 clientes). `--nGeneraciones 2253` con `--poblacion 887` equivale al presupuesto de 2.000.000 de evaluaciones:

```
make run Instancias/5-InstanciasE_redefinidas/e-Instancia44_veh3_cap15_rel1.dat -- \
    --semilla 7 \
    --poblacion 887 \
    --nGeneraciones 2253 \
    --maxTrips 3 \
    --lambdaCap 41.6121 \
    --lambdaT 50.1507 \
    --rcl 0.0547 \
    --probCruzamiento 0.5455 \
    --probMutDeposito 0.6474 \
    --probMutClientes 0.1113 \
    --probMutIntercambio 0.0327 \
    --cierresFraccion 0.1913 \
    --cierresOlvido 0.0513 \
    --cierresPiso 0.0107 \
    --cierresGenSesgo 129 \
    --mejoresFraccion 0.2048 \
    --mejoresTam 96 \
    --mejoresProb 0.9799
```

Los mismos comandos sirven con `./main` en lugar de `make run`, quitando el `--` suelto.

### Parámetros

Todos son opcionales: el que no se entrega toma su valor por defecto. Un nombre desconocido, un valor que falta o un valor fuera de rango detienen el programa con un mensaje.

| Opción | Por defecto | Significado |
|---|---|---|
| `--semilla` | sorteada | Semilla del generador aleatorio. Con la misma semilla la ejecución se repite idéntica. |
| `--poblacion` | 50 | Tamaño de la población. |
| `--nGeneraciones` | 10000 | Criterio de término: máximo de generaciones. |
| `--maxTrips` | 3 | Máximo de trips por route; `0` = sin límite. |
| `--lambdaCap` | 1 | Castigo por unidad de exceso de carga. |
| `--lambdaT` | 1 | Castigo por unidad de exceso de tiempo. |
| `--rcl` | 0.6 | Fracción de candidatos más cercanos entre los que sortea la inicialización. |
| `--probCruzamiento` | 0.6 | Probabilidad de que el hijo nazca del cruce; si no, es copia del padre 1. |
| `--probMutDeposito` | 0.2 | Probabilidad de mutar cada depósito. |
| `--probMutClientes` | 0.2 | Probabilidad, por vehículo, de barajar clientes dentro de sus trips. |
| `--probMutIntercambio` | 0.2 | Probabilidad, por vehículo, de intercambiar dos de sus trips. |
| `--cierresFraccion` | 0.2 | Fracción mejor de la población que alimenta la tabla de cierres. |
| `--cierresOlvido` | 0.7 | Factor por el que se multiplica la tabla en cada generación. |
| `--cierresPiso` | 0.1 | Peso mínimo de cada depósito candidato al sortear un retorno. |
| `--cierresGenSesgo` | 20 | Generación desde la que los retornos se sortean con la tabla. |
| `--mejoresFraccion` | 0.2 | Fracción mejor de la población que postula al archivo de mejores. |
| `--mejoresTam` | 50 | Tamaño máximo del archivo de mejores. |
| `--mejoresProb` | 0.2 | Probabilidad de que el segundo padre salga del archivo de mejores y no de la población. |

El algoritmo termina al completar `--nGeneraciones` generaciones.

### Salida

1. Resumen de la instancia y parámetros usados, incluida la semilla.
2. Una línea por generación con el mejor fitness.
3. La mejor solución factible encontrada: la route de cada vehículo con los identificadores de la instancia y los depósitos entre corchetes. Si no hubo ninguna factible, se entrega la de menor fitness y se avisa.
4. Una línea que empieza con `RESULTADO` y resume la ejecución para leerla desde un script:

```
RESULTADO factible=si tiempo=7382 excesoCarga=0 excesoTiempo=0 fitness=7382 generaciones=100 segundos=0.019 semilla=7
```

### Sintonización con irace

Los parámetros se sintonizaron con [irace](https://cran.r-project.org/package=irace), una vez por set. Requiere R con el paquete `irace` instalado (`install.packages("irace")` desde R) y el ejecutable compilado con `make`.

irace se ejecuta desde la carpeta `sintonizacion/`, porque las rutas de sus archivos son relativas a ella:

```
make
cd sintonizacion
irace --scenario scenario-B.txt | tee salida-B.txt    # set B: 5000 ejecuciones, unos 4 minutos
irace --scenario scenario-E.txt | tee salida-E.txt    # set E: 1800 ejecuciones, unos 96 minutos
```

Si el comando `irace` no está en el `PATH`, se encuentra en la carpeta `bin/` del paquete; `Rscript -e 'cat(system.file("bin", package = "irace"))'` muestra su ubicación. Los tiempos corresponden a 8 ejecuciones en paralelo (`parallel` en cada escenario).

| Archivo | Contenido |
|---|---|
| `parameters.txt` | Los 15 parámetros sintonizados y sus rangos. |
| `scenario-B.txt`, `scenario-E.txt` | Escenario de cada set: instancias, presupuesto de ejecuciones, paralelismo y semilla. |
| `instances-B.txt`, `instances-E.txt` | Instancias de entrenamiento: casos 11 a 13 del set B y 41 a 43 del set E. |
| `target-runner` | Ejecuta `../main` con una configuración candidata y entrega su costo a irace. |
| `salida-B.txt`, `salida-E.txt` | Salida de las sintonizaciones realizadas; al final están las mejores configuraciones. |
| `irace-B.Rdata`, `irace-E.Rdata` | Registro completo de cada sintonización, legible desde R. |

`target-runner` fija el presupuesto de evaluaciones según el set (50.000 en B y 2.000.000 en E) y calcula `--nGeneraciones` a partir de la población de cada candidata, de modo que todas gasten lo mismo. El costo que entrega es el tiempo de viaje de la solución, o 1.000.000.000 más los excesos de carga y de tiempo si no encontró una factible.

### Estructura del código

| Archivos | Contenido |
|---|---|
| `main.cpp` | Población inicial, espacio de creencias, bucle de generaciones y entrega del resultado. |
| `parametros` | Lectura de los parámetros de la línea de comandos; valores por defecto. |
| `instancia` | Lectura del `.dat` y permisos de depósito por vehículo. |
| `solucion` | Representación, evaluación, inicialización y corrección del depósito de cierre. |
| `cruzamiento` | `cruzarTrips`. |
| `mutacion` | `mutarDeposito`, `mutarClientes`, `intercambiarTrips`. |
| `creencias` | Actualización de la tabla de cierres y del archivo de mejores. |

Los headers están en `include/` y las implementaciones en `src/`.

## Convenciones de jerga

Se adopta la convención del paper FDMTVRP:

- **Route**: Jornada completa de un vehículo, desde un depósito de inicio (`D_start`) hasta uno de fin (`D_end`). Está compuesta de Trips. Cada vehículo tiene a lo más una route; puede quedar vacía (vehículo no utilizado).
- **Trip**: Cada salida y regreso dentro de la route: parte de un depósito, visita un subconjunto de clientes y retorna a un depósito (`D_in`, o `D_end` si es el último). El depósito de retorno del trip *i* es el de partida del trip *i+1*.
- **V** (`T` en el paper): Índices de trip por vehículo. En las instancias `V = {1,2,3}`, cota del MILP. En el algoritmo el máximo de trips por route es el parámetro `maxTrips` (por defecto `|V|`; `0` = sin límite), porque en el set E con 2 vehículos no hay solución con 3 trips: `2 × 3 × 15 = 90 < 100`.

## Representación

Codificación directa: la solución es un vector con una `Route` por vehículo (índice = vehículo). Una `Route` guarda el depósito de inicio y una lista de `Trip`; cada `Trip` guarda sus clientes en orden y el depósito al que retorna.

```
struct Trip  { vector<int> clientes; int retorno; };
struct Route { int inicio; vector<Trip> trips; };
```

Cada depósito de corte se guarda **una sola vez**: el trip *i+1* parte del `retorno` del trip *i*, y el trip 1 parte de `inicio`. Así la continuidad queda garantizada por construcción y ningún operador puede dejarla inconsistente. La cantidad de trips es `trips.size()`; una route vacía es un vehículo no utilizado. El cierre de la route es el `retorno` del último trip, que debe cumplir `puedeRecargar` y `puedeTerminar` para ese vehículo.

Ejemplo (índices internos; D1, D2 depósitos, C clientes):

```
Route { inicio: D1,
        trips: [ { [C2, C7],     retorno: D1 },
                 { [C4, C3, C5], retorno: D2 },
                 { [C1, C6],     retorno: D1 } ] }
```

Equivale al recorrido `D1 → C2 → C7 → D1 → C4 → C3 → C5 → D2 → C1 → C6 → D1`: tres trips, recarga en D1 y D2, cierre en D1.

## Lectura de instancia: un depósito, tres identificadores

En el mapa hay 3 depósitos. En el archivo `.dat` cada uno aparece tres veces, con un identificador por rol: como depósito de inicio (`D_start`, por ejemplo `377`), como intermedio de recarga (`D_in`, `10377`) y como final de jornada (`D_end`, `100377`). El modelo matemático del paper necesita esa triplicación para escribir restricciones distintas en cada momento de la route; los tiempos de viaje también quedan repartidos por rol (las salidas bajo el ID de inicio, las llegadas bajo el ID intermedio) y no existe ningún arco que salga de un `D_in`. El paper enlaza los roles mediante la relación `R` (uno a uno entre `D_start` y `D_in`) y mediante arcos `D_in → D_end` que en las instancias solo van al mismo depósito con costo 0.

**Decisión:** al leer la instancia se colapsan los tres identificadores en un único índice interno por depósito físico (`idToIdx[377] = idToIdx[10377] = idToIdx[100377]`). El rol deja de ser un nodo distinto y pasa a ser un permiso por vehículo:

```
Vehiculo { puedeSalir[d], puedeRecargar[d], puedeTerminar[d] }   // d = índice del depósito
```

## Manejo de restricciones

| Restricción | Método |
|---|---|
| Continuidad de depósitos entre trips | Codificación restringida: el depósito de corte se guarda una vez (`Trip.retorno`). |
| Cada cliente exactamente una vez | Codificación restringida: los operadores mueven clientes, no los copian. |
| Permisos de depósito por vehículo | Codificación restringida: los operadores solo eligen depósitos permitidos. |
| ≤ `maxTrips` por route, trip sin clientes | Codificación restringida: los operadores no crean trips de más ni dejan trips vacíos. |
| Carga de un trip ≤ `cap` | Penalización (`excesoCarga`). |
| Tiempo de una route ≤ `tMax` | Penalización (`excesoTiempo`). |

Durante la búsqueda la población admite soluciones con exceso de carga o tiempo. La solución entregada se elige entre las factibles (`Evaluacion::factible()`); si hiciera falta, se repara solo al entregar.

## Función de evaluación

```
Evaluacion { tiempo, excesoCarga, excesoTiempo }
fitness = tiempo + λ_cap · excesoCarga + λ_t · excesoTiempo
```

`tiempo` es la suma de `time[i][j]` sobre el recorrido de todas las routes, el mismo objetivo del paper; el paso de un trip al siguiente no suma porque ocurre en el mismo depósito. `evaluar` aborta si la solución usa un arco inexistente (`INF`): es un error estructural, no un exceso penalizable. Los `λ` son parámetros del algoritmo, no del problema; se entregan por línea de comandos.

## Inicialización

Greedy pseudoaleatorio con lista restringida de candidatos (RCL), el mismo constructor del paper, para que las diferencias en resultados se atribuyan al algoritmo cultural y no al punto de partida. `inicializarSolucion` arma la route de cada vehículo en orden (secuencial):

1. Depósito de salida: sorteo uniforme entre los que el vehículo `puedeSalir`.
2. Siguiente cliente: entre los no servidos que caben en `cap`, se ordenan por tiempo desde el nodo actual y se sortea uno de los `rcl · |C|` más cercanos.
3. Cierre del trip: cuando ningún cliente cabe. Retorna al depósito más cercano que `puedeRecargar`; si es el último trip de la route (`maxTrips` alcanzado o sin clientes), además `puedeTerminar`.
4. Sin cupo: si se agotan los vehículos y quedan clientes, van al último trip con exceso de carga. La estructura siempre es válida; `cap` y `tmax` los paga el fitness.

Parámetros: `rcl`, `maxTrips`, semilla. Misma semilla, misma población. Alternativas descartadas: inserción más barata con pesos por individuo (Li et al. 2016, asigna cada cliente al depósito más cercano, que es lo que el FDMTVRP deja libre), ahorros en dos fases, sweep (sin coordenadas), giant tour + split.
