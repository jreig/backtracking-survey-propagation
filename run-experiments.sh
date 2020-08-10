#!/bin/bash

# seed=24206;
# N=(500,6000,12000,25000)
# alpha=(3.5,3.7,3.9,4.1,4.2,4.21,4.22,4.23,4.24)
# Q=(0.3,0.5,0.7,0.85)
# g=(random,community)

seed=24206;
N=(6000)
alpha=(3.5, 3.7, 3.9, 4.1, 4.2, 4.21, 4.22, 4.23, 4.24)
Q=(0.3, 0.5, 0.7, 0.85)
g=(random,community)

make clean-all
make

# random
for n in "${N[@]}";do
  for a in "${alpha[@]}";do
    ./build/experiment $n $a random $seed
  done;
done;

# community
for n in "${N[@]}";do
  for a in "${alpha[@]}";do
    for q in "${Q[@]}";do
      ./build/experiment $n $a community $q $seed
    done;
  done;
done;

