// OptFrame - Optimization Framework

// Copyright (C) 2009-2015
// http://optframe.sourceforge.net/
//
// This file is part of the OptFrame optimization framework. This framework
// is free software; you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License v3 as published by the
// Free Software Foundation.

// This framework is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License v3 for more details.

// You should have received a copy of the GNU Lesser General Public License v3
// along with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
// USA.

#ifndef OPTFRAME_REDUCED_VNS_HPP_
#define OPTFRAME_REDUCED_VNS_HPP_

#include <math.h>
#include <vector>

#include "VariableNeighborhoodSearch.hpp"
#include "VNS.h"

namespace optframe
{

template<XESolution XES, XEvaluation XEv = Evaluation<>>
class ReducedVNS: public VariableNeighborhoodSearch<XES, XEv>
{
public:

	typedef VariableNeighborhoodSearch<XES, XEv> super;

	//ReducedVNS(Evaluator<S>& evaluator, Constructive<S>& constructive, vector<NS<XES, XEv>*> vshake, vector<NSSeq<S>*> vsearch) :
   ReducedVNS(sref<Evaluator<XES>> evaluator, sref<InitialSearch<XES, XEv>> constructive, vsref<NS<XES, XEv>> vshake, vsref<NSSeq<XES, XEv>> vsearch) :
		VariableNeighborhoodSearch<XES, XEv> (evaluator, constructive, vshake, vsearch)
	{
	}

	virtual ~ReducedVNS()
	{
	}

	virtual LocalSearch<XES, XEv>& buildSearch(unsigned k_search)
	{
		return * new EmptyLocalSearch<XES, XEv>();
	}

	virtual string id() const
	{
		return idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << VariableNeighborhoodSearch<XES, XEv>::idComponent() << "RVNS";
		return ss.str();
	}
};


template<XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<XES>>
class ReducedVNSBuilder : public ILS, public SingleObjSearchBuilder<S, XEv, XES>
{
public:
	virtual ~ReducedVNSBuilder()
	{
	}

	virtual SingleObjSearch<XES>* build(Scanner& scanner, HeuristicFactory<S, XEv, XES, X2ES>& hf, string family = "")
	{
		sptr<GeneralEvaluator<XES>> eval;
		hf.assign(eval, *scanner.nextInt(), scanner.next()); // reads backwards!

		//Constructive<S>* constructive;
      sptr<InitialSearch<XES, XEv>> constructive;
		hf.assign(constructive, *scanner.nextInt(), scanner.next()); // reads backwards!

		vsptr<NS<XES, XEv>> _shakelist;
		hf.assignList(_shakelist, *scanner.nextInt(), scanner.next()); // reads backwards!
      vsref<NS<XES, XEv>> shakelist;
      for(auto x : _shakelist)
         shakelist.push_back(x);

		vsptr<NSSeq<XES, XEv>> _searchlist;
		hf.assignList(_searchlist, *scanner.nextInt(), scanner.next()); // reads backwards!
      vsref<NSSeq<XES, XEv>> searchlist;
      for(auto x : _searchlist)
         searchlist.push_back(x);

		return new BasicVNS<XES, XEv>(eval, constructive, shakelist, searchlist);
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		params.push_back(make_pair(GeneralEvaluator<XES>::idComponent(), "evaluation function"));
		//params.push_back(make_pair(Constructive<S>::idComponent(), "constructive heuristic"));
      params.push_back(make_pair(InitialSearch<XES, XEv>::idComponent(), "constructive heuristic"));

		stringstream ss;
		ss << NS<XES, XEv>::idComponent() << "[]";
		params.push_back(make_pair(ss.str(), "list of NS"));

		stringstream ss2;
		ss2 << NSSeq<XES, XEv>::idComponent() << "[]";
		params.push_back(make_pair(ss2.str(), "list of NSSeq"));

		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == BasicVNS<XES, XEv>::idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << SingleObjSearchBuilder<S, XEv>::idComponent() << VNS::family() << "RVNS";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /*OPTFRAME_REDUCED_VNS_HPP_*/
