// SPDX-License-Identifier:  MIT OR LGPL-3.0-or-later
// Copyright (C) 2007-2024 - OptFrame developers
// https://github.com/optframe/optframe

// OptFrame Demo TSP - Simulated Annealing

#include <iostream>
#include <fstream>  
#include <ctime>    
#include <chrono>   
#include "TSP-fcore.hpp"
#include <OptFrame/Core.hpp>
#include <OptFrame/Heuristics/Heuristics.hpp>  // many metaheuristics here...
#include <OptFrame/Heuristics/SA/BasicSimulatedAnnealing.hpp> // Simulated Annealing
#include <OptFrame/LocalSearch.hpp>

// import everything on main()
using namespace std;        // NOLINT
using namespace optframe;   // NOLINT
using namespace scannerpp;  // NOLINT

int main(int argc, char* argv[]) {
  srand(time(0));  // current time as seed

  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <instance file>" << std::endl;
    return 1;
  }

  const char* instanceFile = argv[1];
  string instanceName = instanceFile;
  size_t lastSlashPos = instanceName.find_last_of("/\\");
  if (lastSlashPos != string::npos) {
    instanceName = instanceName.substr(lastSlashPos + 1);
  }

  // Load data into problem context 'pTSP'
  ifstream infile(instanceFile);
  if (!infile) {
    std::cerr << "Error: Could not open instance file " << instanceFile << std::endl;
    return 1;
  }

  Scanner scanner{&infile};
  sref<ProblemContext> pTSP{new ProblemContext{}};
  
  try {
    pTSP->load(scanner);
  } catch (const std::exception& e) {
    std::cerr << "Error: Exception occurred while loading instance file: " << e.what() << std::endl;
    return 1;
  }

  //commented for performance
  //std::cout << pTSP->dist << std::endl;

  OptFrameDemoTSP demo{pTSP};

  // Create simple solution
  std::vector<int> initialSolution = *demo.randomConstructive->generateSolution(0);
  double initialCost = demo.eval->evaluate(initialSolution).evaluation();

  // Evaluation value and store on ESolution pair
  ESolutionTSP esol(initialSolution, demo.eval->evaluate(initialSolution));
  std::cout << "Initial evaluation: ";
  esol.second.print();  // print evaluation

  // Simulated Annealing parameters
  double initialTemp = 100000.0; // Temperatura inicial
  double finalTemp = 0.1;    // Temperatura final
  double coolingRate = 0.98;    // Taxa de resfriamento
  int maxIterations = 20000;    // Número máximo de iterações

  // Initialize necessary components
  sref<InitialSearch<ESolutionTSP>> initialSearch(new BasicInitialSearch<ESolutionTSP>(demo.randomConstructive, demo.eval));
  sref<NS<ESolutionTSP>> ns(demo.nsSwap);
  sref<RandGen> rg(new RandGen());

  // Measure execution time
  auto start = chrono::high_resolution_clock::now();

  // Initialize Simulated Annealing
  BasicSimulatedAnnealing<ESolutionTSP> sa(demo.eval, initialSearch, ns, initialTemp, maxIterations, coolingRate, rg);

  
  // Perform the search with a condition to stop when temperature reaches finalTemp
  auto status = sa.search(StopCriteria<ESolutionTSP::second_type>(finalTemp));
  ESolutionTSP best = *status.best;

  auto end = chrono::high_resolution_clock::now();
  auto duration = chrono::duration_cast<chrono::microseconds>(end - start).count();

  // Print instance name and SA parameters
  cout << "Instance name: " << instanceName << ";" << endl;
  cout << "SA variables: initialTemp = " << initialTemp << "; finalTemp = " << finalTemp << "; maxIterations = " << maxIterations << "; coolingRate = " << coolingRate << ";" << endl;

  // Print initial solution and evaluation
  cout << "Initial evaluation: " << initialCost << ";" << endl;
  cout << "Initial solution: vector(" << initialSolution.size() << ") [";
  for (size_t i = 0; i < initialSolution.size(); ++i) {
    cout << initialSolution[i];
    if (i != initialSolution.size() - 1) {
      cout << " , ";
    }
  }
  cout << "];" << endl;

  // Print best solution value
  cout << "Best solution found by Simulated Annealing: ";
  best.second.print();
  cout << "Solution: vector(" << best.first.size() << ") [";
  for (size_t i = 0; i < best.first.size(); ++i) {
    cout << best.first[i];
    if (i != best.first.size() - 1) {
      cout << " , ";
    }
  }
  cout << "];" << endl;

  // Calculate and print improvement
  double finalCost = best.second.evaluation();
  double improvement = ((initialCost - finalCost) / initialCost) * 100;
  cout << "Improvement: " << improvement << "%;" << endl;

  // Print execution time
  cout << "Execution time: " << duration << " microseconds;" << endl;

  std::cout << "FINISHED" << std::endl;
  return 0;
}