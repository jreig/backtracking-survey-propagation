# Contents

This project is part of the Master's Final Project entitled "Survey Propagation
in real-world SAT instances" belonging to the Master's Degree in Artificial
Intelligence Research organized by the UIMP and AEPIA.

- [Objective](#objective)
- [FactorGraph](#factorgraph)
- [Algorithms](#algorithms)
  - [Survey Propagation](#survey-propagation)
  - [Unit Propagation](#unit-propagation)
  - [Walksat](#walksat)
  - [Survey Inspired Decimation](#survey-inspired-decimation)
  - [Backtracking Survey Propagation](#backtracking-survey-propagation)
- [Experiments](#experiments)
- [Develop](#develop)
- [Test](#test)
- [References](#references)

# Objective

The main objective of this project is to implement the Backtraking Survey Propagation
algorithm (BSP) and the Survey Inspired Decimation algorithm (SID), evaluate them and
test the results against those obtained in \[[1](#references)\].

# FactorGraph

Both algorithms use a graph as a representation of a CNF. In order to be able to
execute SID and BSP, a custom graph has been implemented. This graph can enable
and disable its nodes and edges when a variable is assigned in order to simplify
the graph (and the corresponding CNF). To be compatible with backtracking, every
assignation can be stored in an AssignationStep and can be reverted.

A FactorGraph is initialized from a DIMACS file and contains the following components:

**Variable** -
Represents a variable of the CNF. Has a unique identifier and can be assigned
to a value (true|false). It contains a list of Edges that connect the variable to
all clauses where it appears and can store an evaluation value.

**Clause** -
Represents a clause of the CNF. Has a unique identifier and can be enabled or disabled.
Every clause contains a list of Edges that connect the clause with all the
variables that appear in it.

**Edge** -
Represents the connection of a variable and a clause with a literal type
(false = negated literal, true otherwise) that indicate if the variable _i_
present in the clause _a_ is negated or not.
It can be enabled or disabled and can store a survey value.

**AssignmentStep** -
Represents the assignment of variables values. Store a list of assigned Variables,
a list of disabled clauses and a list of disabled Edges. This object store all
the information needed to revert an assigment and be able to perform backtracking.

# Algorithms

## Survey Propagation

Survey Propagation (SP) is the base algorithm for BSP and DIS. It is a message
passing algorithm that obtains, for every edge a->i in the factor graph, the
probability that a warning is send from a to i. \[[1](#references)\]

The algorithm runs until converges or the maximum iterations are reached.
The FactorGraph is modified in every iteration storing the survey values in the Edges.

```
INPUT: FactorGraph, maxIterations, epsilon
OUTPUT: True if converged, false if unconverged

0. For every Edge, randomly initialize the survey value with a real number [0,1]
1. For iteration t = 0 to t = maxIterations:
   1.1 Shuffle the Edge list in a random order
   1.2 Update sequentialy the survey values on all Edges using SP-UPDATE
   1.3 If the survey value in the previous iteration minus the new one is
       less than epsilon on all edges, return true
2. If t = maxIterations, return false
```

**SP-UPDATE**

Subrutine to update the survey value of an Edge with the survey values of the
neighbour Edges using equations 26 and 27 from \[[1](#references)\]

## Unit Propagation

Unit Propagation (UP) is an algorithm to simplify a CNF that looks for clauses
with only one literal and assigns the variable with the value that satisfy the
clause \[[2](#references)\]. Then, simplifies the graph and repeats the process.
The algorithm runs until the CNF is solved, a contradiction is found or the CNF
can't be reduced.

The FactorGraph is modified by assigning variables and dissabling Clauses and Edges.
The AssigmentStep is also modified by storing the previous information.

```
INPUT: FactorGraph, AssignmentStep
OUTPUT: True if all went ok, false if a contradiction was found

1. Found all enabled Clauses with only one enabled Edge and assign the Variable
   of the Edge to true if Edge is POSITIVE and to false if NEGATIVE.
   Return true if no unitary Clauses are found.
2. For each Clause:
   2.1 Disable the clause if is satisfied by the assignment (contains the
       assigned literal)
   2.2 Disable each Edge of the clause that contain an assigned Variable with
       the oposite literal type.
       If the Clause have 0 enabled Edges, return false (contradiction found).
3. Go to step 1
```

## Walksat

Walksat is a local search algorithm to solve SAT problems that starts with a
random assignments and perform a number of flips considering if the flip will
unsatisfy previously satisfied clauses and introducing noise \[[2](#references)\].

The FactorGraph is modified by assigning variables and dissabling Clauses and Edges.
The AssigmentStep is also modified by storing the previous information.

```
INPUT: FactorGraph, maxFlips, maxTries, noise, AssignmentStep
OUTPUT: True if a satisfying assigment is found, false otherwise

1. For try t = 0 to maxTries:
   1.1 Assign all Variables with a random value.
   1.2 For flip f = 0 to maxFlips:
       1.2.1 If FactorGraph is satisfied, return true.
       1.2.2 Randomly select an unsatisfied clause and calculate the break-count
             of its variables.
       1.2.3 Flip a Variable of the Clause if has break-count = 0.
             If not, with probability p (noise), flip a random variable and
             with probability 1 - p, flip the variable with lowest break-count.
2. If a sat assignment was not found, return false.
```

## Survey Inspired Decimation

Survey Inspired Decimation (SID) is an iterative algorithm that use the fixed-point
surveys of SP to select and assign variables, which allow to reduce the graph \[[1](#references)\].
The algorithm runs until all variables are assigned, SP can't converge or a
cntradiction is found. If all the surveys in SP are trivial, walksat is used.

The FactorGraph is modified by assigning variables and dissabling Clauses and Edges.

```
INPUT: FactorGraph, assignmentFraction, SP Params, Walksat Params
OUTPUT: True if SAT, false if UNSAT or SP don't converge

1. Run UNIT PROPAGTION. If a contradiction in found, return false.
   If SAT, return true. Otherwise, continue with the algorithm.
2. Run SP. If does not converge return false.
3. Decimate:
   3.1 If all surveys are trivial, return WALKSAT result
   3.2 Otherwise, evaluate all variables, assign a set of them (assignmentFraction)
       and clean the graph.
4. Go to step 1.
```

## Backtracking Survey Propagation

-- TODO --

# Experiments

As stablished previously, the main objective of this project is to evaluate BSP
and SID and test the results against those obtained in \[[1](#references)\].

in order to do so, the following experiments are done:

### Base Experiment

-- TODO --

This experiment executes the SID algorithm on 50 random 3-SAT CNF with multiple
configurations to obtain the percentage of CNF that can be solved.

Configurations:

- N (variables) = 25000, 50000, 100000
- α (clauses/variables ratio) = 4.21, 4.22, 4.23, 4.24
- f (assignment fraction) = 4%, 2%, 1%, .5%, .25%, .125%

### Backtracking Experiment

-- TODO --

This experiment executes the BSP algorithm on 50 random 3-SAT CNF with multiple
configurations to obtain the percentage of CNF that can be solved.

Configurations:

- N (variables) = 25000, 50000, 100000
- α (clauses/variables ratio) = 4.21, 4.22, 4.23, 4.24
- f (assignment fraction) = 4%, 2%, 1%, .5%, .25%, .125%

# Develop

-- TODO --

All code has been developed in Pop!\_OS 19.10 (linux)

Dependencies:

- make build system
- c++17 compiler

# Test

To run the test execute the following commands:

`make build-test`

`make run-test`

# References

- \[1\] [Survey Propagation - An algorithm for satisfiability](https://arxiv.org/abs/cs/0212002v4)
- \[2\] [Handbook of satisfiability](https://www.amazon.es/Satisfiability-Frontiers-Artificial-Intelligence-Applications/dp/1586039296/ref=sr_1_1?__mk_es_ES=%C3%85M%C3%85%C5%BD%C3%95%C3%91&dchild=1&keywords=Handbook+of+satisfiability&qid=1586367798&sr=8-1)
