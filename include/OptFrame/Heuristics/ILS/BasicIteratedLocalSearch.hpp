// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HEURISTICS_ILS_BASICITERATEDLOCALSEARCH_HPP_
#define OPTFRAME_HEURISTICS_ILS_BASICITERATEDLOCALSEARCH_HPP_

#include <math.h>

#include <string>
#include <utility>
#include <vector>

#include "../../LocalSearch.hpp"
#include "BasicILSPerturbation.hpp"
#include "ILS.h"
#include "IteratedLocalSearch.hpp"

namespace optframe {

typedef int BasicHistory;

template <XESolution XES, XEvaluation XEv = Evaluation<>>
class BasicIteratedLocalSearch
    : public IteratedLocalSearch<BasicHistory, XES, XEv> {
 protected:
  sref<LocalSearch<XES, XEv>> ls;
  sref<BasicILSPerturbation<XES, XEv>> p;
  int iterMax;

 public:
  BasicIteratedLocalSearch(sref<GeneralEvaluator<XES, XEv>> e,
                           sref<InitialSearch<XES>> constructive,
                           sref<LocalSearch<XES, XEv>> _ls,
                           sref<BasicILSPerturbation<XES, XEv>> _p,
                           int _iterMax)
      : IteratedLocalSearch<BasicHistory, XES, XEv>(e, constructive),
        ls(_ls),
        p(_p),
        iterMax(_iterMax) {}

  virtual ~BasicIteratedLocalSearch() {}

  sref<BasicHistory> initializeHistory() override {
    sref<int> iter = new int;
    iter = 0;

    return iter;
  }

  void localSearch(XES& se, const StopCriteria<XEv>& sosc) override {
    ls->searchFrom(se, sosc);
  }

  void perturbation(XES& se, const StopCriteria<XEv>& sosc,
                    sref<BasicHistory> history) override {
    int iter = history;

    p->perturb(se, sosc);

    // Incrementa a iteracao
    iter++;

    // Atualiza o historico
    history = iter;
  }

  bool acceptanceCriterion(const Evaluation<>& e1, const Evaluation<>& e2,
                           sref<BasicHistory> history) override {
    if (IteratedLocalSearch<BasicHistory, XES, XEv>::evaluator->betterStrict(
            e1, e2)) {
      // =======================
      //   Melhor solucao: 's2'
      // =======================
      cout << "Best fo: " << e1.evaluation();
      cout << " on [iter " << history << "]" << endl;

      // =======================
      //  Atualiza o historico
      // =======================
      // iter = 0
      history = 0;

      // =======================
      //    Retorna s2
      // =======================
      return true;
    } else {
      return false;
    }
  }

  bool terminationCondition(sref<BasicHistory> history) override {
    int iter = history;

    return (iter >= iterMax);
  }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (SingleObjSearch<XES>::compatible(s));
  }

  std::string id() const override { return idComponent(); }

  static string idComponent() {
    stringstream ss;
    ss << IteratedLocalSearch<BasicHistory, XES, XEv>::idComponent()
       << "BasicILS";
    return ss.str();
  }
};

template <XSolution S, XEvaluation XEv = Evaluation<>,
          XESolution XES = pair<S, XEv>,
          X2ESolution<XES> X2ES = MultiESolution<XES>>
class BasicIteratedLocalSearchBuilder
    : public ILS,
      public SingleObjSearchBuilder<S, XEv, XES> {
 public:
  ~BasicIteratedLocalSearchBuilder() override = default;

  SingleObjSearch<XES>* build(Scanner& scanner,
                              HeuristicFactory<S, XEv, XES, X2ES>& hf,
                              string family = "") override {
    sptr<GeneralEvaluator<XES, XEv>> eval;
    std::string comp_id1 = scanner.next();
    int id1 = *scanner.nextInt();
    hf.assign(eval, id1, comp_id1);

    sptr<InitialSearch<XES>> constructive;
    std::string comp_id2 = scanner.next();
    int id2 = *scanner.nextInt();
    hf.assign(constructive, id2, comp_id2);

    string rest = scanner.rest();

    pair<sptr<LocalSearch<XES, XEv>>, std::string> method;
    method = hf.createLocalSearch(rest);

    sptr<LocalSearch<XES, XEv>> h = method.first;

    scanner = Scanner(method.second);

    sptr<BasicILSPerturbation<XES, XEv>> pert;
    std::string comp_id3 = scanner.next();
    int id3 = *scanner.nextInt();
    hf.assign(pert, id3, comp_id3);

    int iterMax = *scanner.nextInt();

    // NOLINTNEXTLINE
    return new BasicIteratedLocalSearch<XES, XEv>(eval, constructive, h, pert,
                                                  iterMax);
  }

  vector<pair<std::string, std::string>> parameters() override {
    vector<pair<string, string>> params;
    params.push_back(make_pair(GeneralEvaluator<XES, XEv>::idComponent(),
                               "evaluation function"));
    params.push_back(
        make_pair(InitialSearch<XES>::idComponent(), "constructive heuristic"));
    params.push_back(
        make_pair(LocalSearch<XES, XEv>::idComponent(), "local search"));
    params.push_back(make_pair(BasicILSPerturbation<XES, XEv>::idComponent(),
                               "ils perturbation"));
    params.push_back(make_pair("OptFrame:int",
                               "max number of iterations without improvement"));

    return params;
  }

  bool canBuild(std::string component) override {
    return component == BasicIteratedLocalSearch<XES, XEv>::idComponent();
  }

  static string idComponent() {
    stringstream ss;
    ss << SingleObjSearchBuilder<S, XEv>::idComponent() << ":" << ILS::family()
       << "BasicILS";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_ILS_BASICITERATEDLOCALSEARCH_HPP_
