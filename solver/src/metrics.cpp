#include <constants.hpp>
#include <fstream>
#include <metrics.hpp>

namespace sat {
  void Metrics::saveMetrics(const std::string& path) {
    const std::string filePath = path + constants::MetricsFileExtension;

    // Create file if not exists
    if (!std::ifstream(filePath)) {
      std::ofstream newFile(filePath);
      newFile
          << "variables,clauses,backtracking,decimation_fraction,sp_max_it,sp_epsilon,ws_noise,ws_max_tries,ws_max_flip_c,seed,result,total_time,sp_total_it,sid_total_it,ws_total_flips"
          << std::endl;
      newFile.close();
    }

    // Append metrics to file
    std::ofstream metricsFile(filePath, std::ios::app);
    metricsFile << variables << "," << clauses << "," << backtrackingEnabled << "," << decimationFraction << ","
                << SP_MaxIt << "," << SP_Epsilon << "," << WS_Noise << "," << WS_MaxTries << "," << WS_MaxFlipC << ","
                << seed << "," << result << "," << totalSeconds << "," << SP_TotalIt << "," << SID_TotalIt << ","
                << WS_TotalFlips << std::endl;
    metricsFile.close();
  }
}  // namespace sat