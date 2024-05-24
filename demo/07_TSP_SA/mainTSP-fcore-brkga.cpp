// SPDX-License-Identifier: MIT OR LGPL-3.0-or-later
// Copyright (C) 2007-2022 - OptFrame developers
// https://github.com/optframe/optframe

// OptFrame Demo TSP - BRKGA

// C++
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <ctime>
#include "TSP-fcore.hpp"
// implementation of TSP
//
#include <OptFrame/Core.hpp>
#include <OptFrame/Heuristics/EA/RK/BRKGA.hpp>
#include <OptFrame/Heuristics/Heuristics.hpp>  // many metaheuristics here...
#include <OptFrame/InitialPopulation.hpp>
#include <OptFrame/LocalSearch.hpp>
#include <OptFrame/Timer.hpp>

// import everything on main()
using namespace std;        // NOLINT
using namespace optframe;   // NOLINT
using namespace scannerpp;  // NOLINT
// using namespace TSP_fcore;

class MyRandomKeysInitEPop : public InitialEPopulation<std::pair<std::vector<double>, Evaluation<int>>>
{
    using RSK = std::vector<double>;

private:
    int sz;
    sref<RandGen> rg;

public:
    explicit MyRandomKeysInitEPop(int size, sref<RandGen> _rg = new RandGen)
        : sz{size}, rg{_rg} {}

    bool canEvaluate() const override { return false; }

    VEPopulation<std::pair<RSK, Evaluation<int>>> generateEPopulation(unsigned populationSize, double timelimit) override
    {
        VEPopulation<std::pair<RSK, Evaluation<int>>> pop;

        for (unsigned i = 0; i < populationSize; i++)
        {
            vector<double> vd(sz);
            for (int j = 0; j < sz; j++) vd[j] = (rg->rand() % 100000) / 100000.0;
            std::pair<RSK, Evaluation<int>> ind{vd, Evaluation<int>{}};
            pop.push_back(ind);
        }

        return pop;
    }
};

pair<Evaluation<int>, vector<int>> fDecodeEval(sref<Evaluator<typename ESolutionTSP::first_type, typename ESolutionTSP::second_type, ESolutionTSP>> eval, const vector<double>& rk)
{
    vector<pair<double, int>> v(rk.size());
    for (unsigned i = 0; i < v.size(); i++) v[i] = pair<double, int>(rk[i], i);

    sort(v.begin(), v.end(), [](const pair<double, int>& i, const pair<double, int>& j) -> bool {
        return i.first < j.first;
    });

    vector<int> p(v.size());
    for (unsigned i = 0; i < v.size(); i++) p[i] = v[i].second;

    Evaluation<int> e = eval->evaluate(p);
    return make_pair(e, p);
}

int main()
{
    sref<RandGen> rg = new RandGen;  // avoids weird windows OS interactions

    // Start timer
    Timer t;

    // Load data into problem context 'pTSP'
    ifstream file("instances/08_TRP-S1000-R1.tsp");
    if (!file) {
        cerr << "Error: could not open file instances/01_berlin52.tsp" << endl;
        return 1;
    }
    Scanner scanner(&file);
    sref<ProblemContext> pTSP{new ProblemContext{}};
    pTSP->load(scanner);
    std::cout << pTSP->dist << std::endl;

    OptFrameDemoTSP demo{pTSP};
    // Setup decoder function
    demo.decoder = sptr<DecoderRandomKeys<ESolutionTSP, double>>{
        new FDecoderEvalRK<std::pair<std::vector<int>, Evaluation<int>>, double>{
            demo.eval, fDecodeEval}};

    // Parameters BRKGA
    unsigned population_size = 30;
    unsigned num_generations = 2000;
    double elite_proportion = 0.4;
    double mutant_proportion = 0.3;
    double elite_inheritance_probability = 0.6;

    sref<DecoderRandomKeys<ESolutionTSP, double>> _decoder = demo.decoder;
    sref<InitialEPopulation<std::pair<vector<double>, ESolutionTSP::second_type>>> _initPop = new MyRandomKeysInitEPop(pTSP->n);  // passing key_size

    BRKGA<ESolutionTSP, double> brkga(
        _decoder, MyRandomKeysInitEPop(pTSP->n, rg),  // key_size = pTSP.n
        population_size, num_generations, elite_proportion, mutant_proportion, elite_inheritance_probability, rg);

    // Store initial solution and evaluation
    auto initial_sol = demo.randomConstructive->generateSolution(0);
    ESolutionTSP esol(*initial_sol, demo.eval->evaluate(*initial_sol));
    auto initial_eval = esol.second.evaluation();

    auto searchOut = brkga.search(3000.0);  // 3.0 seconds max
    ESolutionTSP best = *searchOut.best;

    // Calculate improvement
    double improvement = 100.0 * (initial_eval - best.second.evaluation()) / initial_eval;

    // Print results
    std::cout << "Instance: 08_TRP-S1000-R1.tsp" << std::endl;
    std::cout << "BRKGA parameters: population size = " << population_size
              << ", num generations = " << num_generations
              << ", elite proportion = " << elite_proportion
              << ", mutant proportion = " << mutant_proportion
              << ", elite inheritance probability = " << elite_inheritance_probability << std::endl;
    std::cout << "Initial evaluation: " << initial_eval << std::endl;
    std::cout << "Initial solution: [";
    for (size_t i = 0; i < initial_sol->size(); i++) {
        std::cout << (*initial_sol)[i];
        if (i < initial_sol->size() - 1) std::cout << ", ";
    }
    std::cout << "]" << std::endl;
    std::cout << "Best solution found by BRKGA: Evaluation function value = " << best.second.evaluation() << std::endl;
    std::cout << "Best solution: [";
    for (size_t i = 0; i < best.first.size(); i++) {
        std::cout << best.first[i];
        if (i < best.first.size() - 1) std::cout << ", ";
    }
    std::cout << "]" << std::endl;
    std::cout << "Improvement: " << improvement << " %" << std::endl;

    // Print execution time
    std::cout << "Execution Time: " << t.inSecs() << " seconds" << std::endl;

    std::cout << "FINISHED" << std::endl;
    return 0;
}
