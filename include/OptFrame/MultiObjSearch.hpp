// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_MULTIOBJSEARCH_HPP_
#define OPTFRAME_MULTIOBJSEARCH_HPP_

// C++
#include <cstring>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
//
#include <OptFrame/Component.hpp>
#include <OptFrame/Direction.hpp>
#include <OptFrame/Evaluation.hpp>
#include <OptFrame/GlobalSearch.hpp>  // Base class
#include <OptFrame/Helper/MultiEvaluation.hpp>
#include <OptFrame/Helper/Population.hpp>
#include <OptFrame/Hyper/ComponentBuilder.hpp>
#include <OptFrame/MultiEvaluator.hpp>
#include <OptFrame/Pareto.hpp>
#include <OptFrame/ParetoDominance.hpp>
#include <OptFrame/ParetoDominanceWeak.hpp>

// using namespace std;

namespace optframe {

template <class Self, class XES>
concept
#if __cplusplus <= 201703L  // after c++20, not required 'bool'
    bool
#endif
        XMultiObjSearch = requires(Self a) {
  XESolution<XES>;
  X2ESolution<typename Self::BestType, XES>;
};

template <XESolution XMES, XESolution XMES2 = XMES,
          XSearch<XMES2> XMSH2 = XMES2>
class MultiObjSearch : public GlobalSearch<XMES, Pareto<XMES>> {
  using XMSH = Pareto<XMES>;  // PRIMARY/BEST search space
  //
  using S = typename XMES::first_type;
  using XMEv = typename XMES::second_type;
  static_assert(XEvaluation<typename XMEv::XEv>);
  using XEv = typename XMEv::XEv;
  static_assert(XEvaluation<XEv>);

 public:
  // ========================================
  // THIS CLASS IS USELESS! WHAT'S THE POINT?
  // Best to just have XMultiObjSearch
  // ========================================

  MultiObjSearch() {}

  virtual ~MultiObjSearch() = default;

  op<Pareto<XMES>>& getBestPareto() { return this->best; }

  SearchOutput<XMES, Pareto<XMES>> searchBy(
      const StopCriteria<XMEv>& stopCriteria,
      std::optional<Pareto<XMES>> _best) override = 0;

  std::string log() const override { return "Empty heuristic log."; }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (Component::compatible(s));
  }

  static string idComponent() {
    stringstream ss;
    ss << Component::idComponent() << "MultiObjSearch"
       << Domain::getAlternativeDomain<XMSH>("<X2MESf64>") << ":";
    // NOTE THAT: PRIMARY/BEST IS ALWAYS X2MESf64 FOR MULTI-OBJ-SEARCH
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

template <XSolution S, XEvaluation XEv = Evaluation<>,
          XESolution XES = pair<S, XEv>,
          X2ESolution<XES> X2ES = MultiESolution<XES>>
class MultiObjSearchBuilder : public ComponentBuilder<S, XEv, XES, X2ES> {
 public:
  virtual ~MultiObjSearchBuilder() = default;

  // NOLINTNEXTLINE
  virtual MultiObjSearch<XES>* build(Scanner& scanner,
                                     HeuristicFactory<S, XEv, XES, X2ES>& hf,
                                     string family = "") = 0;
  // NOLINTNEXTLINE
  Component* buildComponent(Scanner& scanner,
                            HeuristicFactory<S, XEv, XES, X2ES>& hf,
                            string family = "") override {
    return build(scanner, hf, family);
  }

  vector<pair<string, string>> parameters() override = 0;

  bool canBuild(string) override = 0;

  static string idComponent() {
    stringstream ss;
    ss << ComponentBuilder<S, XEv, XES, X2ES>::idComponent()
       << "MultiObjSearch:";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFRAME_MULTIOBJSEARCH_HPP_