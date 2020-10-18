#pragma once

namespace sat {

// SID parameters
#define DEFAULT_DECIMATION_FRACTION 0.0

// Survey Propagation parameters
#define DEFAULT_SP_MAX_ITERATIONS 1000
#define DEFAULT_SP_EPSILON 0.001

// WALKSAT parameters
#define DEFAULT_WS_MAX_TRIES 100
#define DEFAULT_WS_MAX_FLIPS_COEFICIENT 100
#define DEFAULT_WS_NOISE 0.57

// Solver parameters
#define DEFAULT_SEED 0
#define DEFAULT_METRICS_FILE "metrics.csv"
#define SOLUTION_FILE_EXTENSION ".sol"
}