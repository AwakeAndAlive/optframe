// SPDX-License-Identifier: MIT OR LGPL-3.0-or-later
// Copyright (C) 2007-2024 - OptFrame developers
// https://github.com/optframe/optframe

// OptFrame Demo TSP - Simulated Annealing

#include <iostream>
#include <fstream>
#include <ctime>
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
  std::random_device rd;
  std::mt19937 gen(rd());

  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <instance file>" << std::endl;
    return 1;
  }

  const char* instanceFile = argv[1];

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

  OptFrameDemoTSP demo{pTSP};

  std::cout << "======== Testa Construtivo Aleatório ========" << std::endl;
  std::vector<int> initialSolution = *demo.randomConstructive->generateSolution(0);
  std::cout << initialSolution << std::endl;

  std::cout << "======== Testa Avaliador ========" << std::endl;
  ESolutionTSP esol(initialSolution, demo.eval->evaluate(initialSolution));
  esol.second.print();

  std::cout << "======== Executa Simulated Annealing ========" << std::endl;
  RandGen rg;

  sref<InitialSearch<ESolutionTSP>> initRand(new BasicInitialSearch<ESolutionTSP>(demo.randomConstructive, demo.eval));
  sref<NS<ESolutionTSP>> ns(demo.nsSwap);
  vsref<NS<ESolutionTSP>> neighbors;
  neighbors.push_back(ns);
  sref<GeneralEvaluator<ESolutionTSP>> evaluator(new FEvaluator<ESolutionTSP, MinOrMax::MINIMIZE, ProblemContext>(pTSP, fevaluate));

  BasicSimulatedAnnealing<ESolutionTSP> sa(evaluator, initRand, neighbors, 0.98, 1000, 100000.0, sref<RandGen>(new RandGen()));

  optframe::Timer t;
  auto searchOut = sa.search(StopCriteria<ESolutionTSP::second_type>{3000.0});  // 10.0 seconds max
  std::cout << "spent time: " << t.now() << "s" << std::endl;

  ESolutionTSP melhor = *searchOut.best;
  std::cout << "======== Imprime melhor solução do SA ========" << std::endl;
  cout << melhor.first << endl;
  melhor.second.print();

  std::cout << "======== Fim da Execução ========" << std::endl;
  return 0;
}
