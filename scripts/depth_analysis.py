import sys
import pandas as pd
import numpy as np
import seaborn as sns



if __name__ == "__main__":
    if (len(sys.argv) != 3):
        print("Numero de argumentos incorrecto")

    print("Cargando datos.")
    sns.set_style("white")
    sns.set_context("poster")

    filename = sys.argv[1]
    distance = float(sys.argv[2])
    distances = pd.read_csv(filename)

    dist_serie = distances[distances.columns[0]]

    print(f"Cabecera mediciones: {distances.columns}")
    print("Resumen:")
    print(distances.describe())

    measurement_dist = sns.displot(dist_serie, kde=False, bins=16)
    measurement_dist.set_title(f"Mediciones realizadas a {distance} centímetros")
    measurement_dist.set_xlabel("Centímetros [cm]")
    measurement_dist.set_ylabel("Numero de mediciones")
    measurement_dist.savefig("measurement_dist.png")

    error_dist = measurement_dist - distance
    error_dist.set_title(f"Error medición a {distance} centímetros")
    error_dist.set_xlabel("Centímetros [cm]")
    error_dist.set_ylabel("Cantidad")
    error_dist.savefig("measurement_dist.png")
    print("Resumen error:")
    print(error_dist.describe())