#!/bin/bash

# Parâmetros fixos
initial_temperature=155016
temperature_reduction_factor=0.9806
max_iterations=523

# Lista de instâncias
instances=(
    "01_berlin52.tsp"
    "02_kroD100.tsp"
    "03_pr226.tsp"
    "04_lin318.tsp"
    "05_TRP-S500-R1.tsp"
    "06_d657.tsp"
    "07_rat784.tsp"
    "08_TRP-S1000-R1.tsp"
)
output_csv="tsp-sa-results.csv"

# Limpa o arquivo CSV de saída
> $output_csv

# Executa 30 vezes cada instância
for instance in "${instances[@]}"; do
    for i in {1..30}; do
        output=$(./tsp-sa "instances/$instance" $initial_temperature $temperature_reduction_factor $max_iterations)
        # Salva apenas a última linha de saída, removendo o prefixo 'instances/'
        echo "$output" | tail -n 1 | sed "s|instances/||" >> $output_csv
    done
done
