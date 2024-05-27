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

int main(int argc, char *argv[])
{
    if (argc != 7) {
        cerr << "Usage: " << argv[0] << " <instance> <population_size> <num_generations> <elite_proportion> <mutant_proportion> <elite_inheritance_probability>" << endl;
        return 1;
    }

    string instance = argv[1];
    unsigned population_size = stoi(argv[2]);
    unsigned num_generations = stoi(argv[3]);
    double elite_proportion = stod(argv[4]);
    double mutant_proportion = stod(argv[5]);
    double elite_inheritance_probability = stod(argv[6]);

    // Remove "instances/" prefix from instance name
    string instance_name = instance.substr(instance.find_last_of("/") + 1);

    sref<RandGen> rg = new RandGen;  // avoids weird windows OS interactions

    // Start timer
    Timer t;

    // Load data into problem context 'pTSP'
    ifstream file(instance);
    if (!file) {
        cerr << "Error: could not open file " << instance << endl;
        return 1;
    }
    Scanner scanner(&file);
    sref<ProblemContext> pTSP{new ProblemContext{}};
    pTSP->load(scanner);

    OptFrameDemoTSP demo{pTSP};
    // Setup decoder function
    demo.decoder = sptr<DecoderRandomKeys<ESolutionTSP, double>>{
        new FDecoderEvalRK<std::pair<std::vector<int>, Evaluation<int>>, double>{
            demo.eval, fDecodeEval}};

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

    // Calculate execution time
    double execution_time = t.inSecs();

    // Print results in CSV format
    std::cout << instance_name << ";"
              << initial_eval << ";"
              << best.second.evaluation() << ";"
              << improvement << ";"
              << execution_time << ";{";
    for (size_t i = 0; i < best.first.size(); i++) {
        std::cout << best.first[i];
        if (i < best.first.size() - 1) std::cout << ",";
    }
    std::cout << "}" << std::endl;

    return 0;
}
