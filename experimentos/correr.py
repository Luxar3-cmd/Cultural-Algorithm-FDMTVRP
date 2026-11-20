"""Experimentos finales: ejecuta el algoritmo sobre las instancias de prueba y guarda los resultados.

Uso (desde cualquier carpeta):
    python3 experimentos/correr.py B
    python3 experimentos/correr.py E
    python3 experimentos/correr.py E 4      # con 4 ejecuciones en paralelo en vez de 8

Deja:
    experimentos/resultados-<set>.csv       una fila por ejecución
    experimentos/trazas/<set>/<variante>/   salida completa de cada ejecución (curvas de convergencia)

Si se interrumpe, al relanzarlo salta las ejecuciones que ya están en el CSV.
"""
import csv
import subprocess
import sys
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

AQUI = Path(__file__).resolve().parent
RAIZ = AQUI.parent
EXE = RAIZ / "main"

SEMILLAS = range(1, 31)
RELS = range(1, 7)          # rel7 no está definido en el paper

# Por set: dónde están las instancias, cuáles son de prueba (las que irace no vio),
# el presupuesto de evaluaciones y la configuración que entregó irace.
SETS = {
    "B": {
        "carpeta": RAIZ / "Instancias" / "2-InstanciasB_redefinidas",
        "patron": "b-Instancia{caso}_veh{veh}_cap2_rel{rel}.dat",
        "casos": range(14, 21),
        "vehiculos": (2, 3),
        "presupuesto": 50_000,
        "poblacion": 514,
        "config": "--lambdaCap 186.6979 --lambdaT 10.6254 --rcl 0.5912 --probCruzamiento 0.971"
                  " --probMutDeposito 0.156 --probMutClientes 0.3067 --probMutIntercambio 0.066"
                  " --cierresFraccion 0.2609 --cierresOlvido 0.5075 --cierresPiso 0.027"
                  " --cierresGenSesgo 237 --mejoresFraccion 0.2235 --mejoresTam 179 --mejoresProb 0.4955",
    },
    "E": {
        "carpeta": RAIZ / "Instancias" / "5-InstanciasE_redefinidas",
        "patron": "e-Instancia{caso}_veh{veh}_cap15_rel{rel}.dat",
        "casos": range(44, 51),
        "vehiculos": (3,),
        "presupuesto": 2_000_000,
        "poblacion": 887,
        "config": "--lambdaCap 41.6121 --lambdaT 50.1507 --rcl 0.0547 --probCruzamiento 0.5455"
                  " --probMutDeposito 0.6474 --probMutClientes 0.1113 --probMutIntercambio 0.0327"
                  " --cierresFraccion 0.1913 --cierresOlvido 0.0513 --cierresPiso 0.0107"
                  " --cierresGenSesgo 129 --mejoresFraccion 0.2048 --mejoresTam 96 --mejoresProb 0.9799",
    },
}

# Variantes: banderas que se agregan al final de la configuración. Como el programa lee las
# banderas en orden, una bandera repetida reemplaza a la anterior.
# "sin_cultura" apaga las dos influencias del espacio de creencias: la tabla de cierres nunca
# se activa (generación inalcanzable) y el padre 2 nunca sale del archivo de mejores.
VARIANTES = {
    "completa": "",
    "sin_cultura": "--cierresGenSesgo 1000000000 --mejoresProb 0",
}

COLUMNAS = ["instancia", "variante", "semilla", "factible", "tiempo",
            "excesoCarga", "excesoTiempo", "fitness", "generaciones", "segundos"]


def ejecutar(s, nombre_set, instancia, variante, semilla):
    """Corre una ejecución, guarda su salida completa y devuelve la fila del CSV."""
    # La población inicial también se evalúa: poblacion * (nGeneraciones + 1) <= presupuesto.
    generaciones = s["presupuesto"] // s["poblacion"] - 1
    comando = [str(EXE), str(s["carpeta"] / instancia),
               "--semilla", str(semilla),
               "--poblacion", str(s["poblacion"]),
               "--nGeneraciones", str(generaciones),
               *s["config"].split(), *VARIANTES[variante].split()]
    salida = subprocess.run(comando, capture_output=True, text=True, check=True).stdout

    traza = AQUI / "trazas" / nombre_set / variante / f"{Path(instancia).stem}_s{semilla}.txt"
    traza.parent.mkdir(parents=True, exist_ok=True)
    traza.write_text(salida)

    # Última línea: "RESULTADO factible=si tiempo=9194 excesoCarga=0 ... semilla=1"
    campos = dict(par.split("=") for par in salida.strip().splitlines()[-1].split()[1:])
    return {**campos, "instancia": instancia, "variante": variante}


def main():
    nombre_set = sys.argv[1]
    hilos = int(sys.argv[2]) if len(sys.argv) > 2 else 8
    s = SETS[nombre_set]

    instancias = [s["patron"].format(caso=c, veh=v, rel=r)
                  for c in s["casos"] for v in s["vehiculos"] for r in RELS]
    tareas = [(i, v, sem) for v in VARIANTES for i in instancias for sem in SEMILLAS]

    # Lo que ya está en el CSV no se repite.
    ruta_csv = AQUI / f"resultados-{nombre_set}.csv"
    hechas = set()
    if ruta_csv.exists():
        with open(ruta_csv) as f:
            hechas = {(fila["instancia"], fila["variante"], int(fila["semilla"])) for fila in csv.DictReader(f)}
    pendientes = [t for t in tareas if t not in hechas]
    print(f"Set {nombre_set}: {len(instancias)} instancias, {len(tareas)} ejecuciones, {len(pendientes)} pendientes")

    with open(ruta_csv, "a", newline="") as f, ThreadPoolExecutor(max_workers=hilos) as pool:
        escritor = csv.DictWriter(f, fieldnames=COLUMNAS)
        if not hechas:
            escritor.writeheader()
        resultados = pool.map(lambda t: ejecutar(s, nombre_set, *t), pendientes)
        for n, fila in enumerate(resultados, start=1):
            escritor.writerow(fila)
            f.flush()
            if n % 50 == 0 or n == len(pendientes):
                print(f"  {n}/{len(pendientes)}")


if __name__ == "__main__":
    main()
