Este es el primer push

Decisiones que debo tomar:

- Con qué algoritmo me voy a comparar. (Yo creo que con el del paper FDMTVRP)
- Representación de la solución
- Inicialización de la población
- Cruzamiento de la población
- Mutación de la población

Experimental mente:
1. Definir qué responder (Pregunta)
  Es mi algoritmo mejor que los mejores conocidos para el problema?
  Es mi variante de algoritmo mejor que otra variante similar?
  Para este problema, ¿Funciona mejor ILS, SA o Búsqueda Tabú?
  ¿El operador de reparación que agregué genera una mejora en el desempeño?

  Contra quién debería compararme, qué instancias utilizar, que tipo de algoritmo, etc.

2. Cómo diseñar el experimento (Diseño)

---

Contra quién te comparas en el paper puede ser contra el estado del arte (A), mi familia de algoritmos (B), otras caracteristicas (C).

---

# Implementación de algoritmo cultural

## Compilación y ejecución

Requiere Linux, `g++` con C++17 y `make`.

```
make                               # compila y deja el ejecutable ./main
./main <ruta a la instancia .dat>  # ejecuta el algoritmo sobre esa instancia
make clean                         # borra el ejecutable y la carpeta build/
```

Ejemplo, con las instancias descomprimidas en `Instancias/`:

```
./main Instancias/2-InstanciasB_redefinidas/b-Instancia11_veh2_cap2_rel1.dat
./main Instancias/5-InstanciasE_redefinidas/e-Instancia41_veh3_cap15_rel1.dat
```

`make run <ruta a la instancia>` compila si hace falta y ejecuta en un solo paso.

La salida muestra el resumen de la instancia, la semilla usada y el mejor fitness de cada generación. La instancia es el único argumento de la línea de comandos por ahora.

### Parámetros

Se asignan en la sección 2 de `main.cpp`; tras cambiarlos hay que volver a compilar con `make`. La semilla se sortea en cada ejecución y se imprime.

| Parámetro | Valor | Significado |
|---|---|---|
| `poblacion` | 100 | Tamaño de la población. |
| `nGeneraciones` | 100 | Cantidad de generaciones; es el criterio de término. |
| `maxTrips` | 3 | Máximo de trips por route; `0` = sin límite. |
| `lambdaCap`, `lambdaT` | 1, 1 | Castigo por exceso de carga y por exceso de tiempo. |
| `rcl` | 0.6 | Fracción de candidatos más cercanos entre los que sortea la inicialización. |
| `probCruzamiento` | 0.6 | Probabilidad de que el hijo nazca del cruce; si no, es copia del padre 1. |
| `probMutDeposito` | 0.2 | Probabilidad de mutar cada depósito. |
| `probMutClientes` | 0.2 | Probabilidad, por vehículo, de barajar clientes dentro de sus trips. |
| `probMutIntercambio` | 0.2 | Probabilidad, por vehículo, de intercambiar dos de sus trips. |
| `cierresFraccion` | 0.2 | Fracción mejor de la población que alimenta la tabla de cierres. |
| `cierresOlvido` | 0.7 | Factor por el que se multiplica la tabla en cada generación. |
| `cierresPiso` | 0.1 | Peso mínimo de cada depósito candidato al sortear un retorno. |
| `cierresGenSesgo` | 20 | Generación desde la que los retornos se sortean con la tabla. |
| `mejoresFraccion` | 0.2 | Fracción mejor de la población que postula al archivo de mejores. |
| `mejoresTam` | 50 | Tamaño máximo del archivo de mejores. |

### Estructura del código

| Archivos | Contenido |
|---|---|
| `main.cpp` | Parámetros, población inicial, espacio de creencias y bucle de generaciones. |
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
