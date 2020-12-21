#pragma once

#include <string>

namespace sat::constants {
  // SID parameters
  const bool Default_SID_BacktrackingEnabled = false;
  const double Default_SID_DecimationFraction = 0.0;

  // SP parameters
  const unsigned Default_SP_MaxIterations = 1000;
  const double Default_SP_Epsilon = 0.001;

  // WALKSAT parameters
  const double Default_WS_Noise = 0.57;
  const unsigned Default_WS_MaxTries = 100;
  const unsigned Default_WS_MaxFlipsCoeficient = 100;

  // Solver parameters
  const unsigned long Default_Seed = 0;
  const std::string MetricsFileExtension = ".csv";
  const std::string SolutionFileExtension = ".sol";

  // Command line arguments help strings
  const std::string CNF_FileHelp = "File in DIMACS format of the CNF to be solved.";

  const std::string SID_BacktrackingEnabledHelp = "Flag to enable the use of backtracking in the SID algorithm.";
  const std::string SID_DecimationFractionHelp =
      "Fraction of unsaigned variables that will be assigned in the decimation process of the SID algorithm.";

  const std::string SP_MaxIterationsHelp = "SP maximum number of iterations.";
  const std::string SP_EpsilonHelp = "SP convergence threshold.";

  const std::string WS_NoiseHelp = "WALKSAT noise value";
  const std::string WS_MaxTriesHelp = "WALKSAT maximum tries.";
  const std::string WS_MaxFlipsCoeficientHelp =
      "Coeficient used to calculate WALKSAT maximum flips by multiplying it by the number of variables in the CNF.";

  const std::string SeedHelp = "Seed value for reproducibility.";
}  // namespace sat::constants