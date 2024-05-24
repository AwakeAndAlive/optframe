// SPDX-License-Identifier: MIT OR LGPL-3.0-or-later
// Copyright (C) 2007-2024 - OptFrame developers
// https://github.com/optframe/optframe

// OptFrame Demo TSP - Iterated Local Search

#include <iostream>
#include <fstream>
#include <ctime>
#include "TSP-fcore.hpp"

#include <OptFrame/Core.hpp>
#include <OptFrame/Heuristics/Heuristics.hpp>
#include <OptFrame/Heuristics/ILS/IteratedLocalSearchLevels.hpp>
#include <OptFrame/Heuristics/LocalSearches/BestImprovement.hpp>
#include <OptFrame/Heuristics/LocalSearches/VariableNeighborhoodDescent.hpp>
#include <OptFrame/LocalSearch.hpp>
#include <OptFrame/Timer.hpp>

using namespace std;        
using namespace optframe;   
using namespace scannerpp;  

int main(int argc, char* argv[])
{
    if (argc < 7) {
        std::cerr << "Usage: " << argv[0] << " <instance_file> <max_failed_iterations> <max_perturbation_level> <num_neighborhoods> <local_search_depth> <time_limit>" << std::endl;
        return 1;
    }

    string instance_file = argv[1];
    int max_failed_iterations = std::stoi(argv[2]);
    int max_perturbation_level = std::stoi(argv[3]);
    int num_neighborhoods = std::stoi(argv[4]);
    int local_search_depth = std::stoi(argv[5]);
    double time_limit = std::stod(argv[6]);

    Timer t; // Start timer

    srand(time(nullptr));  // Seed random generator with current time

    // Load data into problem context
    ifstream file(instance_file);
    if (!file) {
        std::cerr << "Error: could not open file " << instance_file << std::endl;
        return 1;
    }
    Scanner scanner(&file);
    sref<ProblemContext> pTSP{new ProblemContext{}};
    pTSP->load(scanner);
    std::cout << pTSP->dist << std::endl;

    OptFrameDemoTSP demo{pTSP};

    // Create initial solution
    std::vector<int> sol = *demo.randomConstructive->generateSolution(0);
    ESolutionTSP esol(sol, demo.eval->evaluate(sol));
    esol.second.print();  // Print evaluation

    // Store initial solution and evaluation
    auto initial_sol = esol.first;
    auto initial_eval = esol.second.evaluation();

    // Swap 0 with 1
    MoveSwap move{demo.pTSP, make_pair(0, 1), fApplySwap};
    move.print();

    // Move for solution 'esol'
    auto m1 = demo.nsSwap->randomMove(esol);
    m1->print();

    std::cout << "begin listing NSSeqSwapFancy" << std::endl;
    auto it1 = demo.nsseqSwap->getIterator(esol);
    for (it1->first(); !it1->isDone(); it1->next()) it1->current()->print();
    std::cout << "end listing NSSeqSwapFancy" << std::endl;

    // Random number generator
    RandGen rg;
    sref<RandGen> rg2{new RandGen};

    BasicInitialSearch<ESolutionTSP> initRand(demo.randomConstructive, demo.eval);

    // Create list of local searches
    vsref<LocalSearch<ESolutionTSP>> ns_list;
    Evaluator<ESolutionTSP::first_type, ESolutionTSP::second_type>* ev2 =
        new FEvaluator<ESolutionTSP, MinOrMax::MINIMIZE, ProblemContext>{
            pTSP, fevaluate};
    GeneralEvaluator<ESolutionTSP>* gev2 = (GeneralEvaluator<ESolutionTSP>*)ev2;
    sref<GeneralEvaluator<ESolutionTSP>> eval2(gev2);

    for (int i = 0; i < num_neighborhoods; i++) {
        ns_list.push_back(new BestImprovement<ESolutionTSP>(eval2, demo.nsseqSwap)); // Add different neighborhoods if needed
    }

    VariableNeighborhoodDescent<ESolutionTSP> VND(demo.eval, ns_list);

    ILSLPerturbationLPlus2<ESolutionTSP> pert(demo.eval, demo.nsSwap, rg2);

    IteratedLocalSearchLevels<ESolutionTSP> ils(demo.eval, initRand, VND, pert, max_failed_iterations, max_perturbation_level);

    std::cout << "will start ILS for " << time_limit << " seconds" << std::endl;

    auto status = ils.search(
        StopCriteria<ESolutionTSP::second_type>{time_limit});  // Time limit
    ESolutionTSP best = *status.best;
    // Print best solution value
    best.second.print();
    std::cout << "solution: " << best.first << std::endl;

    // Calculate improvement
    double improvement = 100.0 * (initial_eval - best.second.evaluation()) / initial_eval;

    // Print results
    std::cout << "Instance name: " << instance_file << std::endl;
    std::cout << "ILS variables: max_failed_iterations = " << max_failed_iterations
              << "; max_perturbation_level = " << max_perturbation_level
              << "; num_neighborhoods = " << num_neighborhoods
              << "; local_search_depth = " << local_search_depth
              << "; time_limit = " << time_limit << " seconds;" << std::endl;
    std::cout << "Initial evaluation: " << initial_eval << std::endl;
    std::cout << "Initial solution: vector(" << initial_sol.size() << ") [";
    for (size_t i = 0; i < initial_sol.size(); i++) {
        std::cout << initial_sol[i];
        if (i < initial_sol.size() - 1) std::cout << " , ";
    }
    std::cout << "]" << std::endl;
    std::cout << "Best solution found by ILS: Evaluation function value = " << best.second.evaluation() << std::endl;
    std::cout << "Solution: vector(" << best.first.size() << ") [";
    for (size_t i = 0; i < best.first.size(); i++) {
        std::cout << best.first[i];
        if (i < best.first.size() - 1) std::cout << " , ";
    }
    std::cout << "]" << std::endl;
    std::cout << "Improvement: " << improvement << "%" << std::endl;

    // Print execution time
    std::cout << "Execution Time: " << t.inSecs() << " seconds" << std::endl;

    return 0;
}
