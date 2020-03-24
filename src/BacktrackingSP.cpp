#include <BacktrackingSP.hpp>

using namespace std;

namespace bsp {

BacktrackingSP::BacktrackingSP(float threshold, int maxIterations) {
  threshold = threshold;
  maxIterations = maxIterations * CLOCKS_PER_SEC;
  graph = nullptr;
};

BacktrackingSP::~BacktrackingSP() { delete graph; }

BSPResult BacktrackingSP::BSP(ifstream& file) {
  // Initialize graph
  if (!file.is_open()) {
    cerr << "ERROR: Can't open file" << endl;
    graph = nullptr;
  } else {
    // Delete previous graph if any
    if (graph != nullptr) delete graph;

    graph = new FactorGraph(file);
    graph->Print();
  }

  // Check correct initialization
  if (graph == nullptr) {
    cerr << "ERROR: FactorGraph not initialized" << endl;
    return ERROR;
  }

  // Initialize surveys
  random_device rd;
  mt19937 gen(rd());
  uniform_real_distribution<> dis(0, 1);
  for (Edge* edge : graph->edges) {
    edge->survey = dis(gen);
  }

  // Loop until maxIterations or convergence
  bool converged = false;
  clock_t t = clock();
  int i = 0;
  while (i < maxIterations && !converged) {
    // Shuffle edges
    std::shuffle(graph->edges.begin(), graph->edges.end(), gen);

    bool allConverged = true;
    for (Edge* edge : graph->edges) {
      // Survey value in the previous iteration
      float lastSurvValue = edge->survey;

      // Update survey
      UpdateSurvey(edge);

      // Check if survey converged
      bool hasConverged = abs(edge->survey - lastSurvValue) < threshold;
      if (!hasConverged) allConverged = false;
    }

    converged = allConverged;
    i++;
  };

  t = clock() - t;

  // If algorithm didn't converged means we reach max time
  if (!converged) return UNCONVERGED;

  cout << "Survey propagation converged successfully in "
       << (float)(t / CLOCKS_PER_SEC) << " seconds" << endl;

  return CONVERGED;
};

void BacktrackingSP::UpdateSurvey(Edge* ai) {
  // Param edge is a->i
  float Sai = 1.0f;

  // For each a->j when j != i
  for (Edge* aj : ai->clausule->neighbourEdges) {
    if (aj == ai) continue;  // Pointer comparsion

    // Product values initalization for all b->j survey values
    float Pubj = 1.0f;
    float Psbj = 1.0f;
    float P0bj = 1.0f;

    // For each b->j when b != a // TODO: investigate std::accumulate
    for (Edge* bj : aj->literal->neighbourEdges) {
      if (bj == aj) continue;  // Pointer comparion

      Pubj = Pubj * (1 - bj->survey);
      Psbj = Psbj * (1 - bj->survey);
      P0bj = P0bj * (1 - bj->survey);
    }

    // Product values for all a->j survey values
    float Puaj = (1.0f - Pubj) * Psbj;
    float Psaj = (1.0f - Psbj) * Pubj;
    float P0aj = P0bj;

    // Update a->i survey value
    Sai = Sai * (Puaj / (Puaj + Psaj + P0aj));
  }

  // update a->i survey
  cout << Sai << endl;
  ai->survey = Sai;
};

}  // namespace bsp