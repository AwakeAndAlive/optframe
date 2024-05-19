// SPDX-License-Identifier:  MIT OR LGPL-3.0-or-later
// Copyright (C) 2007-2022 - OptFrame developers
// https://github.com/optframe/optframe

// OptFrame Demo TSP - Simulated Annealing

// C++
#include <iostream>
//
#include "TSP-fcore.hpp"
// implementation of TSP

#include <OptFrame/Core.hpp>
#include <OptFrame/Heuristics/Heuristics.hpp>  // many metaheuristics here...
#include <OptFrame/Heuristics/SA/BasicSimulatedAnnealing.hpp> // Simulated Annealing
#include <OptFrame/LocalSearch.hpp>

// import everything on main()
using namespace std;        // NOLINT
using namespace optframe;   // NOLINT
using namespace scannerpp;  // NOLINT
// using namespace TSP_fcore;

int main() {
  srand(0);  // using system random (weak... just an example!)

  // Load data into problem context 'pTSP'
  Scanner scanner{"5\n1 10 10\n2 20 20\n3 30 30\n4 40 40\n5 50 50\n"};
  sref<ProblemContext> pTSP{new ProblemContext{}};
  pTSP->load(scanner);
  std::cout << pTSP->dist << std::endl;

  OptFrameDemoTSP demo{pTSP};

  // Create simple solution
  std::vector<int> sol = *demo.randomConstructive->generateSolution(0);
  std::cout << "Initial solution: " << sol << std::endl;

  // Evaluation value and store on ESolution pair
  ESolutionTSP esol(sol, demo.eval->evaluate(sol));
  std::cout << "Initial evaluation: ";
  esol.second.print();  // print evaluation

  // Simulated Annealing parameters
  double initial_temp = 1000.0; // Temperatura inicial
  double cooling_rate = 0.95;   // Taxa de resfriamento
  int max_iterations = 1000;    // Número máximo de iterações

  // Initialize necessary components
  sref<InitialSearch<ESolutionTSP>> initialSearch(new BasicInitialSearch<ESolutionTSP>(demo.randomConstructive, demo.eval));
  sref<NS<ESolutionTSP>> ns(demo.nsSwap);
  sref<RandGen> rg(new RandGen());

  // Initialize Simulated Annealing
  BasicSimulatedAnnealing<ESolutionTSP> sa(demo.eval, initialSearch, ns, initial_temp, max_iterations, cooling_rate, rg);

  // Perform the search
  auto status = sa.search(max_iterations);
  ESolutionTSP best = *status.best;

  // Print best solution value
  std::cout << "Best solution found by Simulated Annealing: ";
  best.second.print();
  std::cout << "Solution: " << best.first << std::endl;

  std::cout << "FINISHED" << std::endl;
  return 0;
}
