#!/bin/bash

# Seed: 2420659006

./libs/cnf-generator/generate-random.sh 5000 3.5
./build/experiment 5000 3.5 random 2420659006 | tee ./experiments/result/result-random-5000-3.5-2420659006.txt

./libs/cnf-generator/generate-random.sh 5000 3.7
./build/experiment 5000 3.7 random 2420659006 | tee ./experiments/result/result-random-5000-3.7-2420659006.txt

./libs/cnf-generator/generate-random.sh 5000 3.9
./build/experiment 5000 3.9 random 2420659006 | tee ./experiments/result/result-random-5000-3.9-2420659006.txt

./libs/cnf-generator/generate-random.sh 5000 4.1
./build/experiment 5000 4.1 random 2420659006 | tee ./experiments/result/result-random-5000-4.1-2420659006.txt

./libs/cnf-generator/generate-random.sh 5000 4.16
./build/experiment 5000 4.16 random 2420659006 | tee ./experiments/result/result-random-5000-4.16-2420659006.txt

./libs/cnf-generator/generate-random.sh 5000 4.18
./build/experiment 5000 4.18 random 2420659006 | tee ./experiments/result/result-random-5000-4.18-2420659006.txt

./libs/cnf-generator/generate-random.sh 5000 4.2
./build/experiment 5000 4.2 random 2420659006 | tee ./experiments/result/result-random-5000-4.2-2420659006.txt

./libs/cnf-generator/generate-random.sh 5000 4.21
./build/experiment 5000 4.21 random 2420659006 | tee ./experiments/result/result-random-5000-4.21-2420659006.txt

./libs/cnf-generator/generate-random.sh 5000 4.22
./build/experiment 5000 4.22 random 2420659006 | tee ./experiments/result/result-random-5000-4.22-2420659006.txt

./libs/cnf-generator/generate-random.sh 5000 4.23
./build/experiment 5000 4.23 random 2420659006 | tee ./experiments/result/result-random-5000-4.23-2420659006.txt

./libs/cnf-generator/generate-random.sh 5000 4.24
./build/experiment 5000 4.24 random 2420659006 | tee ./experiments/result/result-random-5000-4.24-2420659006.txt