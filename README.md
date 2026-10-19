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

## Convenciones de jerga

Se adopta la convención del paper FDMTVRP:

- **Route**: Jornada completa de un vehículo, desde un depósito de inicio (`D_start`) hasta uno de fin (`D_end`). Está compuesta de Trips. Cada vehículo tiene a lo más una route; puede quedar vacía (vehículo no utilizado).
- **Trip**: Cada salida y regreso dentro de la route: parte de un depósito, visita un subconjunto de clientes y retorna a un depósito (`D_in`, o `D_end` si es el último). El depósito de retorno del trip *i* es el de partida del trip *i+1*.
- **V** (`T` en el paper): Índices de trip por vehículo. En las instancias `V = {1,2,3}`; para comparar con el MILP se impone un máximo de 3 trips por route.

## Representación

La representación elegida para este trabajo, entrega la siguiente información:

- Cantidad de Trips
- Qué visita cada Trip (Depósito, Cliente)

Ejemplo:

\[(D1 C2 C7 D1),(D1 C4 C3 C5 D2),(D2 C1 C6 D1)\]

Sería una Route de la cual se desprenden 3 trips, cada uno visitando distintos clientes y depósitos en el orden establecido.

## Lectura de instancia: un depósito, tres identificadores

En el mapa hay 3 depósitos. En el archivo `.dat` cada uno aparece tres veces, con un identificador por rol: como depósito de inicio (`D_start`, por ejemplo `377`), como intermedio de recarga (`D_in`, `10377`) y como final de jornada (`D_end`, `100377`). El modelo matemático del paper necesita esa triplicación para escribir restricciones distintas en cada momento de la route; los tiempos de viaje también quedan repartidos por rol (las salidas bajo el ID de inicio, las llegadas bajo el ID intermedio) y no existe ningún arco que salga de un `D_in`. El paper enlaza los roles mediante la relación `R` (uno a uno entre `D_start` y `D_in`) y mediante arcos `D_in → D_end` que en las instancias solo van al mismo depósito con costo 0.

**Decisión:** al leer la instancia se colapsan los tres identificadores en un único índice interno por depósito físico (`idToIdx[377] = idToIdx[10377] = idToIdx[100377]`). El rol deja de ser un nodo distinto y pasa a ser un permiso por vehículo:

```
Vehiculo { puedeSalir[d], puedeRecargar[d], puedeTerminar[d] }   // d = índice del depósito
```



