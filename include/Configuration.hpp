#pragma once

// =============================================================================
// Global configuration for the experiment
//
// IMPORTANT: Run make clean-all and build again after changing this config
// =============================================================================

// Survey Propagation parameters
#define SP_MAX_ITERATIONS 1000
#define SP_EPSILON 0.001f

// WALKSAT parameters
#define WS_MAX_TRIES 100
#define WS_MAX_FLIPS 100 * 1000
#define WS_NOISE 0.5f

// CNF instances
#define CNF_INSTANCES 50
