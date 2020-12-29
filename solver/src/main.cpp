#include <cmdparser/cmdparser.hpp>
#include <constants.hpp>
#include <factorgraph/factorgraph.hpp>
#include <iostream>
#include <string>

using namespace std;
using namespace sat::constants;

void configure_parser(cli::Parser &parser) {
  parser.set_required<string>("f", "cnf-file", CNF_FileHelp);

  parser.set_optional<bool>("b", "backtracking", Default_SID_BacktrackingEnabled, SID_BacktrackingEnabledHelp);
  parser.set_optional<double>("d", "decimation-fraction", Default_SID_DecimationFraction, SID_DecimationFractionHelp);

  parser.set_optional<unsigned>("i", "sp-iterations", Default_SP_MaxIterations, SP_MaxIterationsHelp);
  parser.set_optional<double>("e", "sp-epsilon", Default_SP_Epsilon, SP_EpsilonHelp);

  parser.set_optional<double>("n", "ws-noise", Default_WS_Noise, WS_NoiseHelp);
  parser.set_optional<unsigned>("t", "ws-tries", Default_WS_MaxTries, WS_MaxTriesHelp);
  parser.set_optional<unsigned>("c", "ws-flips-coeficient", Default_WS_MaxFlipsCoeficient, WS_MaxFlipsCoeficientHelp);

  parser.set_optional<unsigned long>("s", "seed", Default_Seed, SeedHelp);
}

int main(int argc, char *argv[]) {
  // Parse arguments
  cli::Parser parser(argc, argv);
  configure_parser(parser);
  parser.run_and_exit_if_error();

  string cnf_file = parser.get<string>("f");

  bool use_backtracking = parser.get<bool>("b");
  double sid_fraction = parser.get<double>("d");

  unsigned sp_max_iterations = parser.get<unsigned>("i");
  double sp_epsilon = parser.get<double>("e");

  double ws_noise = parser.get<double>("n");
  unsigned ws_max_tries = parser.get<unsigned>("t");
  unsigned ws_max_flips_coeficient = parser.get<unsigned>("c");

  unsigned long seed = parser.get<unsigned long>("s");

  // configure application

  // read dimcs file and create factor graph
  sat::FactorGraph *graph = new sat::FactorGraph(cnf_file);

  // run solver

  // process results

  return 0;
}