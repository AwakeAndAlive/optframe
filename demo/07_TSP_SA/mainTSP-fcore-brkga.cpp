// SPDX-License-Identifier:  MIT OR LGPL-3.0-or-later
// Copyright (C) 2007-2022 - OptFrame developers
// https://github.com/optframe/optframe

// OptFrame Demo TSP - BRKGA

// C++
#include <iostream>
//
#include "TSP-fcore.hpp"
// implementation of TSP
//
#include <OptFrame/Core.hpp>
#include <OptFrame/Heuristics/EA/RK/BRKGA.hpp>
#include <OptFrame/Heuristics/Heuristics.hpp>  // many metaheuristics here...
#include <OptFrame/InitialPopulation.hpp>
#include <OptFrame/LocalSearch.hpp>

// import everything on main()
using namespace std;        // NOLINT
using namespace optframe;   // NOLINT
using namespace scannerpp;  // NOLINT
// using namespace TSP_fcore;

class MyRandomKeysInitEPop
    : public InitialEPopulation<
          std::pair<std::vector<double>, Evaluation<int>>> {
  using RSK = std::vector<double>;

 private:
  int sz;
  sref<RandGen> rg;

 public:
  explicit MyRandomKeysInitEPop(int size, sref<RandGen> _rg = new RandGen)
      : sz{size}, rg{_rg} {}

  // copy constructor
  // MyRandomKeysInitEPop(const MyRandomKeysInitEPop& self)
  //     : sz{self.sz}, rg{self.rg} {}

  // this generator cannot evaluate solutions
  bool canEvaluate() const override { return false; }

  VEPopulation<std::pair<RSK, Evaluation<int>>> generateEPopulation(
      unsigned populationSize, double timelimit) override {
    VEPopulation<std::pair<RSK, Evaluation<int>>> pop;

    for (unsigned i = 0; i < populationSize; i++) {
      vector<double> vd(sz);
      for (int j = 0; j < sz; j++) vd[j] = (rg->rand() % 100000) / 100000.0;
      // assert(!this->canEvaluate());
      std::pair<RSK, Evaluation<int>> ind{vd, Evaluation<int>{}};
      pop.push_back(ind);
    }

    return pop;
  }
};


pair<Evaluation<int>, vector<int>> fDecodeEval(
    sref<Evaluator<typename ESolutionTSP::first_type,
                   typename ESolutionTSP::second_type, ESolutionTSP>>
        eval,
    const vector<double>& rk) {
  vector<pair<double, int>> v(rk.size());
  //
  for (unsigned i = 0; i < v.size(); i++) v[i] = pair<double, int>(rk[i], i);

  sort(v.begin(), v.end(),
       [](const pair<double, int>& i, const pair<double, int>& j) -> bool {
         return i.first < j.first;
       });

  // R = vector<int>
  vector<int> p(v.size());
  for (unsigned i = 0; i < v.size(); i++) p[i] = v[i].second;

  /*
  // ========== CHECKER ========
  vector<bool> vb(v.size(), false);
  for (unsigned i = 0; i < p.size(); i++)
     vb[p[i]] = true;
  for (unsigned i = 0; i < vb.size(); i++) {
     if (!vb[i]) {
        std::cout << "ERROR rk:" << rk << std::endl;
        std::cout << "ERROR v:" << v << std::endl;
        std::cout << "ERROR p:" << p << std::endl;
        std::cout << "ERROR vb:" << vb << std::endl;
     }
     assert(vb[i]);
  }
  // ===== end CHECKER =====
*/

  Evaluation<int> e = eval->evaluate(p);
  return make_pair(e, p);
}

// evaluator random keys (for TSP)
// FDecoderEvalRK<std::pair<std::vector<int>, Evaluation<int>>, double>
// decoder{fDecode};
int main() {
  sref<RandGen> rg = new RandGen;  // avoids weird windows OS interactions

  // load data into problem context 'pTSP'
  Scanner scanner{"52\n1 565.0 575.0\n2 25.0 185.0\n3 345.0 750.0\n4 945.0 685.0\n5 845.0 655.0\n6 880.0 660.0\n7 25.0 230.0\n8 525.0 1000.0\n9 580.0 1175.0\n10 650.0 1130.0\n11 1605.0 620.0\n12 1220.0 580.0\n13 1465.0 200.0\n14 1530.0 5.0\n15 845.0 680.0\n16 725.0 370.0\n17 145.0 665.0\n18 415.0 635.0\n19 510.0 875.0\n20 560.0 365.0\n21 300.0 465.0\n22 520.0 585.0\n23 480.0 415.0\n24 835.0 625.0\n25 975.0 580.0\n26 1215.0 245.0\n27 1320.0 315.0\n28 1250.0 400.0\n29 660.0 180.0\n30 410.0 250.0\n31 420.0 555.0\n32 575.0 665.0\n33 1150.0 1160.0\n34 700.0 580.0\n35 685.0 595.0\n36 685.0 610.0\n37 770.0 610.0\n38 795.0 645.0\n39 720.0 635.0\n40 760.0 650.0\n41 475.0 960.0\n42 95.0 260.0\n43 875.0 920.0\n44 700.0 500.0\n45 555.0 815.0\n46 830.0 485.0\n47 1170.0 65.0\n48 830.0 610.0\n49 605.0 625.0\n50 595.0 360.0\n51 1340.0 725.0\n52 1740.0 245.0\n"};
  sref<ProblemContext> pTSP{new ProblemContext{}};
  pTSP->load(scanner);
  std::cout << pTSP->dist << std::endl;

  OptFrameDemoTSP demo{pTSP};
  // setup decoder function
  demo.decoder = sptr<DecoderRandomKeys<ESolutionTSP, double>>{
      new FDecoderEvalRK<std::pair<std::vector<int>, Evaluation<int>>, double>{
          demo.eval, fDecodeEval}};

  // Parameters BRKGA
  // (C1): Evaluator<S, XEv>& _evaluator, int key_size, unsigned numGen,
  // unsigned _popSize, double fracTOP, double fracBOT, double _probElitism) :

  sref<DecoderRandomKeys<ESolutionTSP, double>> _decoder = demo.decoder;
  sref<InitialEPopulation<std::pair<vector<double>, ESolutionTSP::second_type>>>
      _initPop = new MyRandomKeysInitEPop(pTSP->n);  // passing key_size

  // eprk, pTSP.n, 1000, 30, 0.4, 0.3, 0.6
  BRKGA<ESolutionTSP, double> brkga(
      _decoder, MyRandomKeysInitEPop(pTSP->n, rg),  // key_size = pTSP.n
      30, 1000, 0.4, 0.3, 0.6, rg);

  auto searchOut = brkga.search(3.0);  // 3.0 seconds max
  ESolutionTSP best = *searchOut.best;
  // best solution value
  best.second.print();
  std::cout << "solution: " << best.first << std::endl;

  std::cout << "FINISHED" << std::endl;
  return 0;
}
