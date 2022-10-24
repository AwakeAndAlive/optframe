// OptFrame 4.2 - Optimization Framework
// Copyright (C) 2009-2021 - MIT LICENSE
// https://github.com/optframe/optframe
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef OPTFRAME_REDUCED_VNS_HPP_
#define OPTFRAME_REDUCED_VNS_HPP_

#include <math.h>

#include <vector>

#include "VNS.h"
#include "VariableNeighborhoodSearch.hpp"

namespace optframe {

template <XESolution XES, XEvaluation XEv = Evaluation<>>
class ReducedVNS : public VariableNeighborhoodSearch<XES, XEv> {
 public:
  typedef VariableNeighborhoodSearch<XES, XEv> super;

  //ReducedVNS(Evaluator<S>& evaluator, Constructive<S>& constructive, vector<NS<XES, XEv>*> vshake, vector<NSSeq<S>*> vsearch) :
  ReducedVNS(sref<Evaluator<XES, XEv>> evaluator, sref<InitialSearch<XES>> constructive, vsref<NS<XES, XEv>> vshake, vsref<NSSeq<XES, XEv>> vsearch)
      : VariableNeighborhoodSearch<XES, XEv>(evaluator, constructive, vshake, vsearch) {
  }

  virtual ~ReducedVNS() {
  }

  sref<LocalSearch<XES, XEv>> buildSearch(unsigned k_search) override {
    return new EmptyLocalSearch<XES, XEv>();
  }

  std::string id() const override {
    return idComponent();
  }

  static string idComponent() {
    stringstream ss;
    ss << VariableNeighborhoodSearch<XES, XEv>::idComponent() << "RVNS";
    return ss.str();
  }
};

template <XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<XES>>
class ReducedVNSBuilder : public ILS, public SingleObjSearchBuilder<S, XEv, XES> {
 public:
  virtual ~ReducedVNSBuilder() {
  }

  SingleObjSearch<XES>* build(Scanner& scanner,
                              HeuristicFactory<S, XEv, XES, X2ES>& hf,
                              string family = "") override {
    sptr<GeneralEvaluator<XES>> eval;
    hf.assign(eval, *scanner.nextInt(), scanner.next());  // reads backwards!

    //Constructive<S>* constructive;
    sptr<InitialSearch<XES>> constructive;
    hf.assign(constructive, *scanner.nextInt(), scanner.next());  // reads backwards!

    vsptr<NS<XES, XEv>> _shakelist;
    hf.assignList(_shakelist, *scanner.nextInt(), scanner.next());  // reads backwards!
    vsref<NS<XES, XEv>> shakelist;
    for (auto x : _shakelist)
      shakelist.push_back(x);

    vsptr<NSSeq<XES, XEv>> _searchlist;
    hf.assignList(_searchlist, *scanner.nextInt(), scanner.next());  // reads backwards!
    vsref<NSSeq<XES, XEv>> searchlist;
    for (auto x : _searchlist)
      searchlist.push_back(x);

    return new BasicVNS<XES, XEv>(eval, constructive, shakelist, searchlist);
  }

  vector<pair<std::string, std::string>> parameters() override {
    vector<pair<string, string>> params;
    params.push_back(make_pair(GeneralEvaluator<XES>::idComponent(), "evaluation function"));
    //params.push_back(make_pair(Constructive<S>::idComponent(), "constructive heuristic"));
    params.push_back(make_pair(InitialSearch<XES>::idComponent(), "constructive heuristic"));

    stringstream ss;
    ss << NS<XES, XEv>::idComponent() << "[]";
    params.push_back(make_pair(ss.str(), "list of NS"));

    stringstream ss2;
    ss2 << NSSeq<XES, XEv>::idComponent() << "[]";
    params.push_back(make_pair(ss2.str(), "list of NSSeq"));

    return params;
  }

  bool canBuild(std::string component) override {
    return component == BasicVNS<XES, XEv>::idComponent();
  }

  static string idComponent() {
    stringstream ss;
    ss << SingleObjSearchBuilder<S, XEv>::idComponent() << VNS::family() << "RVNS";
    return ss.str();
  }

  std::string id() const override {
    return idComponent();
  }
};

}  // namespace optframe

#endif /*OPTFRAME_REDUCED_VNS_HPP_*/
