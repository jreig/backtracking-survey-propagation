#include <string>
#include <iostream>

// Project includes
#include <constants.hpp>
#include <cmdparser/cmdparser.hpp>

using namespace std;
using namespace sat;


void configure_parser(cli::Parser& parser) {
	parser.set_required<string>("c", "cnf-file", "CNF file to solve in DIMACS format.");

  parser.set_optional<bool>("b", "backtracking", false, "Uses the backtracking method to solve the CNF.");
  parser.set_optional<string>("m", "metrics-output", DEFAULT_METRICS_FILE, "Metrics output file in csv format. If the file already exists, the metrics are appended.");
  parser.set_optional<unsigned long>("s", "seed", DEFAULT_SEED, "Seed for reproducibility.");

  parser.set_optional<double>("f", "sid-fraction", DEFAULT_DECIMATION_FRACTION, "Fraction of unsaigned variables that will be assigned in the decimation process of the SID algorithm. Value of 0 means one variable at a time.");

	parser.set_optional<unsigned>("i", "sp-max-iterations", DEFAULT_SP_MAX_ITERATIONS, "Survey Propagation maximum number of iterations.");
  parser.set_optional<double>("e", "sp-epsilon", DEFAULT_SP_EPSILON, "Survey Propagation convergence threshold.");

	parser.set_optional<unsigned>("t", "ws-max-tries", DEFAULT_WS_MAX_TRIES, "WALKSAT maximum tries.");
  parser.set_optional<unsigned>("j", "ws-max-flips-c", DEFAULT_WS_MAX_FLIPS_COEFICIENT, "WALKSAT maximum flips coeficient. The number provided will be multiplied by the CNF number of variables to obtain maximum flips.");
	parser.set_optional<double>("n", "ws-noise", DEFAULT_WS_NOISE, "WALKSAT noise.");
}


int main(int argc, char* argv[]) {

// parser arguments
cli::Parser parser(argc, argv);
configure_parser(parser);
parser.run_and_exit_if_error();

string cnf_file = parser.get<string>("c");

bool use_backtracking = parser.get<bool>("b");
string metrics_file = parser.get<string>("m");
unsigned long seed = parser.get<unsigned long>("s");

double sid_fraction = parser.get<double>("f");

unsigned sp_max_iterations = parser.get<unsigned>("i");
double sp_epsilon = parser.get<double>("e");

unsigned ws_max_tries = parser.get<unsigned>("t");
unsigned ws_max_flips = parser.get<unsigned>("j");
double ws_noise = parser.get<double>("n");

// configure application


// read dimcs file and create factor graph

// run solver

// process results

return 0;
}