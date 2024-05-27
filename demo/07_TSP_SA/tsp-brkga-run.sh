#!/bin/bash

# Parâmetros fixos
population_size=38
num_generations=975
elite_proportion=0.3156
mutant_proportion=0.2105
elite_inheritance_probability=0.6474

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
output_csv="output.csv"

# Limpa o arquivo CSV de saída
> $output_csv

# Executa 30 vezes cada instância
for instance in "${instances[@]}"; do
    for i in {1..30}; do
        output=$(./mainRun "instances/$instance" $population_size $num_generations $elite_proportion $mutant_proportion $elite_inheritance_probability)
        # Ignora a primeira linha de saída da biblioteca "RKGA: best fitness at initial population"
        echo "$output" | sed -n '2p' >> $output_csv
        echo "$output" | sed -n '2p'
    done
done
