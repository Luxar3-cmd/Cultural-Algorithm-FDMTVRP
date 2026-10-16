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

Route: Viaje completo de principio a fin de jornada que realiza el conductor. El cual está compuesto de Trips.
Trip: Es cada salida y regreso de un vehículo. 

## Representación

La representación elegida para este trabajo, entrega la siguiente información: 

- Cantidad de Trips
- Qué visita cada Trip (Depósito, Cliente)

Ejemplo: 

\[(D1 C2 C7 D1),(D1 C4 C3 C5 D2),(D2 C1 C6 D1)\]

Sería un Trip del cual se desprenden 3 tours, cada uno visitando disintos clientes y depósitos en el orden establecido. 


