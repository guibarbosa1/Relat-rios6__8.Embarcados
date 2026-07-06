import argparse
import csv
from pathlib import Path

import matplotlib.pyplot as plt


def ler_csv(caminho_csv: Path):
    with caminho_csv.open(newline="", encoding="utf-8") as arquivo:
        leitor = csv.DictReader(arquivo)
        if not leitor.fieldnames or len(leitor.fieldnames) < 2:
            raise ValueError("O CSV precisa ter uma coluna de tempo e pelo menos uma variável para plotar.")

        dados = {nome: [] for nome in leitor.fieldnames}

        for linha in leitor:
            for nome_coluna in leitor.fieldnames:
                valor = linha.get(nome_coluna, "")
                if valor is None or valor == "":
                    raise ValueError(f"Valor vazio encontrado na coluna '{nome_coluna}'.")
                dados[nome_coluna].append(float(valor))

    return dados


def gerar_grafico(caminho_csv: Path):
    dados = ler_csv(caminho_csv)
    coluna_tempo = next(iter(dados))
    tempos = dados[coluna_tempo]

    if not tempos:
        raise ValueError("O CSV não possui pontos para plotar.")

    plt.figure(figsize=(12, 7))

    for nome_coluna, valores in dados.items():
        if nome_coluna == coluna_tempo:
            continue
        plt.plot(tempos, valores, linewidth=1.6, label=nome_coluna)

    plt.title(caminho_csv.stem)
    plt.xlabel(coluna_tempo)
    plt.ylabel("Valor")
    plt.grid(True, alpha=0.3)
    plt.legend()
    plt.tight_layout()

    caminho_png = caminho_csv.with_suffix(".png")
    plt.savefig(caminho_png, dpi=300)
    plt.close()

    print(f"Gráfico salvo em: {caminho_png}")


def main():
    parser = argparse.ArgumentParser(
        description="Lê um CSV e salva um gráfico PNG com todas as variáveis plotadas em linhas contínuas."
    )
    parser.add_argument("csv_file", help="Caminho para o arquivo CSV")
    args = parser.parse_args()

    caminho_csv = Path(args.csv_file)
    if not caminho_csv.exists():
        raise FileNotFoundError(f"Arquivo não encontrado: {caminho_csv}")

    gerar_grafico(caminho_csv)


if __name__ == "__main__":
    main()