#!/bin/bash

./build/experiment 25000 3.5 random 0 | tee ./experiments/result/result-random-25000-3.5-0.txt
./build/experiment 25000 4.21 random 0 | tee ./experiments/result/result-random-25000-4.21-0.txt
./build/experiment 25000 4.22 random 0 | tee ./experiments/result/result-random-25000-4.22-0.txt
./build/experiment 25000 4.23 random 0 | tee ./experiments/result/result-random-25000-4.23-0.txt
./build/experiment 25000 4.24 random 0 | tee ./experiments/result/result-random-25000-4.24-0.txt