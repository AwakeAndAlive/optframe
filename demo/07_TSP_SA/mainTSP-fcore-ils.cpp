// SPDX-License-Identifier:  MIT OR LGPL-3.0-or-later
// Copyright (C) 2007-2022 - OptFrame developers
// https://github.com/optframe/optframe

// OptFrame Demo TSP - Iterated Local Search

// C++
#include <iostream>
//
#include "TSP-fcore.hpp"
// implementation of TSP

#include <OptFrame/Core.hpp>
#include <OptFrame/Heuristics/Heuristics.hpp>  // many metaheuristics here...
#include <OptFrame/Heuristics/ILS/IteratedLocalSearchLevels.hpp>
#include <OptFrame/Heuristics/LocalSearches/BestImprovement.hpp>
#include <OptFrame/Heuristics/LocalSearches/VariableNeighborhoodDescent.hpp>
#include <OptFrame/LocalSearch.hpp>

// import everything on main()
using namespace std;        // NOLINT
using namespace optframe;   // NOLINT
using namespace scannerpp;  // NOLINT
// using namespace TSP_fcore;
int main() {
  srand(0);  // using system random (weak... just an example!)

  // load data into problem context 'pTSP'
  Scanner scanner{"52\n1 565.0 575.0\n2 25.0 185.0\n3 345.0 750.0\n4 945.0 685.0\n5 845.0 655.0\n6 880.0 660.0\n7 25.0 230.0\n8 525.0 1000.0\n9 580.0 1175.0\n10 650.0 1130.0\n11 1605.0 620.0\n12 1220.0 580.0\n13 1465.0 200.0\n14 1530.0 5.0\n15 845.0 680.0\n16 725.0 370.0\n17 145.0 665.0\n18 415.0 635.0\n19 510.0 875.0\n20 560.0 365.0\n21 300.0 465.0\n22 520.0 585.0\n23 480.0 415.0\n24 835.0 625.0\n25 975.0 580.0\n26 1215.0 245.0\n27 1320.0 315.0\n28 1250.0 400.0\n29 660.0 180.0\n30 410.0 250.0\n31 420.0 555.0\n32 575.0 665.0\n33 1150.0 1160.0\n34 700.0 580.0\n35 685.0 595.0\n36 685.0 610.0\n37 770.0 610.0\n38 795.0 645.0\n39 720.0 635.0\n40 760.0 650.0\n41 475.0 960.0\n42 95.0 260.0\n43 875.0 920.0\n44 700.0 500.0\n45 555.0 815.0\n46 830.0 485.0\n47 1170.0 65.0\n48 830.0 610.0\n49 605.0 625.0\n50 595.0 360.0\n51 1340.0 725.0\n52 1740.0 245.0\n"};
  sref<ProblemContext> pTSP{new ProblemContext{}};
  pTSP->load(scanner);
  std::cout << pTSP->dist << std::endl;

  OptFrameDemoTSP demo{pTSP};

  // evaluator
  // TSPEval ev;
  //
  // create simple solution
  // TSPRandom crand;
  //
  std::vector<int> sol = *demo.randomConstructive->generateSolution(0);
  std::cout << sol << std::endl;

  // evaluation value and store on ESolution pair
  ESolutionTSP esol(sol, demo.eval->evaluate(sol));
  esol.second.print();  // print evaluation

  // swap 0 with 1
  MoveSwap move{demo.pTSP, make_pair(0, 1), fApplySwap};
  move.print();

  // NSSwap nsswap;
  // move for solution 'esol'
  auto m1 = demo.nsSwap->randomMove(esol);
  m1->print();

  std::cout << std::endl;
  std::cout << "begin listing NSSeqSwapFancy" << std::endl;
  //
  auto it1 = demo.nsseqSwap->getIterator(esol);
  for (it1->first(); !it1->isDone(); it1->next()) it1->current()->print();
  std::cout << "end listing NSSeqSwapFancy" << std::endl;

  // Random number generator
  RandGen rg;                      // stack version
  sref<RandGen> rg2{new RandGen};  // heap version (safely shared)
  // testing simulated annealing
  BasicInitialSearch<ESolutionTSP> initRand(demo.randomConstructive, demo.eval);

  vsref<LocalSearch<ESolutionTSP>> ns_list;
  Evaluator<ESolutionTSP::first_type, ESolutionTSP::second_type>* ev2 =
      new FEvaluator<ESolutionTSP, MinOrMax::MINIMIZE, ProblemContext>{
          pTSP, fevaluate};
  GeneralEvaluator<ESolutionTSP>* gev2 = (GeneralEvaluator<ESolutionTSP>*)ev2;
  sref<GeneralEvaluator<ESolutionTSP>> eval2(gev2);
  ns_list.push_back(new BestImprovement<ESolutionTSP>(eval2, demo.nsseqSwap));

  VariableNeighborhoodDescent<ESolutionTSP> VND(demo.eval, ns_list);
  // VND.setVerbose();//
ILSLPerturbationLPlus2<ESolutionTSP> pert(demo.eval, demo.nsSwap, rg2);

IteratedLocalSearchLevels<ESolutionTSP> ils(demo.eval, initRand, VND, pert, 3,
                                            3);
 ils.setVerbose();

std::cout << "will start ILS for 3 seconds" << std::endl;

auto status = ils.search(
    StopCriteria<ESolutionTSP::second_type>{50.0});  // 3.0 seconds max
ESolutionTSP best = *status.best;
// best solution value
best.second.print();
std::cout << "solution: " << best.first << std::endl;

std::cout << "FINISHED" << std::endl;
return 0;
}