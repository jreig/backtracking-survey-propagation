#!/bin/bash

k=3;
variables=(25000 50000 100000)
alpha=(4.21 4.22 4.23 4.24)
for n in "${variables[@]}";do
  for ratio in "${alpha[@]}";do 
	  m=$(echo "scale=0; $n*$ratio/1.0"|bc);

    for i in {1..50};do
      seed=`expr "$i" + "$m"`
      echo $seed
      CNF=experiments/instances/random_${k}SAT_${n}N_${ratio}R_${i}.cnf
      ./libs/cnf-generator/random -n $n -m $m -s $seed -o $CNF
    done;
  done;
done;