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
#include <iomanip> // for std::setprecision

// import everything on main()
using namespace std;        // NOLINT
using namespace optframe;   // NOLINT
using namespace scannerpp;  // NOLINT

int main(int argc, char* argv[]) {
  std::random_device rd;
  std::mt19937 gen(rd());

  if (argc != 5) {
    std::cerr << "Usage: " << argv[0] << " <instance file> <initial temperature> <temperature reduction factor> <max iterations>" << std::endl;
    return 1;
  }

  const char* instanceFile = argv[1];
  double initialTemperature = std::stod(argv[2]);
  double temperatureReductionFactor = std::stod(argv[3]);
  int maxIterations = std::stoi(argv[4]);

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
  auto initialEvaluation = demo.eval->evaluate(initialSolution);
  std::cout << initialSolution << std::endl;

  std::cout << "======== Testa Avaliador ========" << std::endl;
  ESolutionTSP esol(initialSolution, initialEvaluation);
  esol.second.print();

  std::cout << "======== Executa Simulated Annealing ========" << std::endl;
  RandGen rg;

  sref<InitialSearch<ESolutionTSP>> initRand(new BasicInitialSearch<ESolutionTSP>(demo.randomConstructive, demo.eval));
  sref<NS<ESolutionTSP>> ns(demo.nsSwap);
  vsref<NS<ESolutionTSP>> neighbors;
  neighbors.push_back(ns);
  sref<GeneralEvaluator<ESolutionTSP>> evaluator(new FEvaluator<ESolutionTSP, MinOrMax::MINIMIZE, ProblemContext>(pTSP, fevaluate));

  BasicSimulatedAnnealing<ESolutionTSP> sa(evaluator, initRand, neighbors, temperatureReductionFactor, maxIterations, initialTemperature, sref<RandGen>(new RandGen()));

  optframe::Timer t;
  auto searchOut = sa.search(StopCriteria<ESolutionTSP::second_type>{3000.0});  // 10.0 seconds max
  double timeSpent = t.now();
  std::cout << "spent time: " << timeSpent << "s" << std::endl;

  ESolutionTSP melhor = *searchOut.best;
  double finalEvaluation = melhor.second.evaluation();
  std::cout << "======== Imprime melhor solução do SA ========" << std::endl;
  cout << melhor.first << endl;
  melhor.second.print();

  // Calcula o percentual de diferença
  double percentDiference = ((initialEvaluation.evaluation() - finalEvaluation) / initialEvaluation.evaluation()) * 100.0;

  // Formata a saída como CSV
  std::cout << instanceFile << ";"
            << std::fixed << std::setprecision(2) << initialEvaluation.evaluation() << ";"
            << std::fixed << std::setprecision(2) << finalEvaluation << ";"
            << std::fixed << std::setprecision(2) << percentDiference << ";"
            << std::fixed << std::setprecision(2) << timeSpent << ";"
            << "[";
  for (size_t i = 0; i < melhor.first.size(); ++i) {
    std::cout << melhor.first[i];
    if (i < melhor.first.size() - 1) {
      std::cout << ", ";
    }
  }
  std::cout << "]" << std::endl;

  return 0;
}
