# backtracking-survey-propagation

This project is part of the Master's Final Project entitled "Survey Propagation
in real-world SAT instances" belonging to the Master's Degree in Artificial
Intelligence Research organized by the UIMP and AEPIA.

## Contents

- [Objective](#objective)
- [FactorGraph](#factorgraph)
- [Algorithms](#algorithms)
  - [Survey Propagation](#survey-propagation)
- [Experiments](#experiments)
- [Develop](#develop)
- [Test](#test)
- [References](#references)

## Objective

The main objective of this project is to implement the Backtraking Survey Propagation
algorithm (BSP) and the Survey Inspired Decimation algorithm (SID), evaluate them and
test the results against those obtained in \[[1](#references)\].

## FactorGraph

Both algorithms use a graph as a representation of a CNF. In order to be able to
execute SID and BSP, a custom graph has been implemented. This graph can enable
and disable its nodes and edges when a variable is assigned and when a clausule
is satisfied in order to simplify the graph (and the corresponding CNF). To be
compatible with backtracking, every assignation is stored and can be reverted.

A FactorGraph is initialized from a DIMACS file and contin the following components:

**Variable** -
Represents a variable of the CNF. Has a unique identifier and can be assigned
to a value (true|false). It contains a list of Edges that connect the variable to
all clausules where it appears.

**Clausule** -
Represents a clausule of the CNF. Has a unique identifier and can be enabled or disabled.
Every clausule contains a list of Edges of all the variables that appear in the clausule.

**Edge** -
Represents the connection of a variable and a clausule with a literal type that
indicate if the variable _i_ present in the clausule _a_ is negated or not.
It can be enabled or disabled and store a _survey_ value.

## Algorithms

### Survey Propagation

Survey propagation (SP) is the base algorithm for BSP and DIS and it is a message
passing algorithm that obtains, for every edge a->i in the factor graph, the
probability that a warning is send from a to i. \[[1](#references)\]

The algorithm runs with a FactorGraph until converges or the maximum iterations
are reached. The graph is modified in every iteration storing the survey values
in the Edges

```
INPUT: FactorGraph, maxIterations, epsilon
OUTPUT: UNCONVERGED | CONVERGED

0. For every edge a->i randomly initialize the survey value with a real number [0,1]
1. For iteration t = 0 to t = maxIterations:
   1.1 Shuffle the edge list in a random order
   1.2 Update sequentialy the survey values on all edges using subrutine SP-UPDATE
   1.3 If the survey value in the previous iteration minus the new one is
       less than epsilon on all edges, return CONVERGED
2. If t = maxIterations return UNCOVERGED
```

**SP-UPDATE**

Subrutine to update the survey value of an edge with the survey values of the
neighbour edges using equations 26 and 27 from \[[1](#references)\]

## Develop

Dependencies:

- make (build)
- gcc (compiler)
- clang (formater)

To build the code:

`make build`

To run the example code:

`make run`

To run the test:

`make run-test`

# References

- \[1\] [Survey Propagation - An algorithm for satisfiability](https://arxiv.org/abs/cs/0212002v4)
- \[2\] [Handbook of satisfiability](https://www.amazon.es/Satisfiability-Frontiers-Artificial-Intelligence-Applications/dp/1586039296/ref=sr_1_1?__mk_es_ES=%C3%85M%C3%85%C5%BD%C3%95%C3%91&dchild=1&keywords=Handbook+of+satisfiability&qid=1586367798&sr=8-1)
