import math
import sys

def gerar_pontos(distancia, n_pontos=40):
    pontos = []
    for i in range(n_pontos):
        angulo = 2 * math.pi * i / n_pontos
        x = distancia * math.cos(angulo)
        z = distancia * math.sin(angulo)
        pontos.append(f'<point x="{x}" y="0" z="{z}"/>')
    return pontos

def main():
    if len(sys.argv) != 2:
        print("Uso: python controlPoints.py <distancia>")
        return

    try:
        distancia = float(sys.argv[1])
    except ValueError:
        print("Erro: a distância deve ser um número.")
        return

    pontos = gerar_pontos(distancia)
    for ponto in pontos:
        print(ponto)

if __name__ == "__main__":
    main()
