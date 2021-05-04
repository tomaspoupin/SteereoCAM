import sys
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns



if __name__ == "__main__":
    if (len(sys.argv) != 3):
        print("Numero de argumentos incorrecto")
        sys.exit(1)

    print("Cargando datos.")
    sns.set_style("white")
    sns.set_context("paper", rc={"font.size":20,"axes.titlesize":22,"axes.labelsize":20}) 

    filename = sys.argv[1]
    distance = float(sys.argv[2])
    distances = pd.read_csv(filename)

    dist_serie = distances[distances.columns[0]]

    print(f"Cabecera mediciones: {distances.columns}")
    print("Resumen:")
    print(distances.describe())

    measurement_dist = sns.histplot(dist_serie, kde=False, bins=16)
    measurement_dist.set_title(f"Mediciones realizadas a {distance} centímetros")
    measurement_dist.set_xlabel("Centímetros [cm]")
    measurement_dist.set_ylabel("Numero de mediciones")
    fig = measurement_dist.get_figure()
    fig.savefig("measurement_dist.png")

    plt.clf()

    error = np.abs(dist_serie - distance)
    error_dist = sns.histplot(error, kde=False, bins=16)
    error_dist.set_title(f"Error medición a {distance} centímetros")
    error_dist.set_xlabel("Centímetros [cm]")
    error_dist.set_ylabel("Cantidad")
    fig = error_dist.get_figure()
    fig.savefig("error_dist.png")
    print("Resumen error:")
    print(error.describe())

    plt.clf()

    norm_error = ((error/distance)*100)
    norm_error_dist = sns.histplot(norm_error, kde=False, bins=16)
    norm_error_dist.set_title(f"Error medición normalizado a {distance} centímetros")
    norm_error_dist.set_xlabel("% Porcentaje")
    norm_error_dist.set_ylabel("Cantidad")
    fig = norm_error_dist.get_figure()
    fig.savefig("norm_error_dist.png")
    print("Resumen error normalizado:")
    print(norm_error.describe())

    print(f"Error: {(error.mean() / distance) * 100}%")
    print(f"STD Error: {(error.std() / distance) * 100}%")