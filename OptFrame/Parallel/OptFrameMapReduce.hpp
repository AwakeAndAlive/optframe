// OptFrame - Optimization Framework

// Copyright (C) 2009, 2010, 2011
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

#ifndef OptFrameMapReduce_HPP_
#define OptFrameMapReduce_HPP_

#include "MapReduce++/MaPI/MaPI.h"

#include "../Heuristic.hpp"
#include "../NSEnum.hpp"
#include "../Evaluator.hpp"

// OptFrame MapReduce Types

//typedef R KeyA;				// initial solution // TODO memory for reevaluation
//typedef (int,(double,double)) A;		// search subspace index (timelimit,target_f)
//typedef int KeyB;				// dummy unique key (integer -1)
//typedef pair<R, double> B;			// best move in each subspace and its cost // TODO //typedef pair<Move<R, M>*, double> B;	// best move in each subspace and its cost
//typedef R C;					// best solution among all subspaces

typedef pair<int,pair<double,double> > RankAndStop; // A -> int double double

template<class R, class M>
class MyMaPISerializer : public MaPI_Serializer<R, RankAndStop, int, pair<R, double> , R>
{
public:

	virtual R KeyA_fromString(string s) = 0;
	virtual string KeyA_toString(R o) = 0;

	virtual RankAndStop A_fromString(string s) = 0;
	virtual string A_toString(RankAndStop o) = 0;

	virtual int KeyB_fromString(string s) = 0;
	virtual string KeyB_toString(int o) = 0;

	virtual pair<R, double> B_fromString(string s) = 0;
	virtual string B_toString(pair<R, double> o) = 0;

	virtual R C_fromString(string s) = 0;
	virtual string C_toString(R o) = 0;

};


template<class R, class M>
class MyMaPIMapper: public MaPI_Mapper<R, RankAndStop, int, pair<R, double> , R>
{
private:
	Evaluator<R, M>& eval;
	Heuristic<R, M>* hmap;
public:
	MyMaPIMapper(MaPI_MapReduce<R, RankAndStop, int, pair<R, double> , R> * mr, MaPI_Serializer<R, RankAndStop, int, pair<R, double> , R> * s, Evaluator<R, M>& _eval) :
		MaPI_Mapper<R, RankAndStop, int, pair<R, double> , R> (mr,s), eval(_eval)
	{
	}

	void setHeuristic(Heuristic<R, M>* h) {hmap = h;}

	virtual vector<pair<int, pair<R, double> > > map(pair<R, RankAndStop> a) // TODO
	{
		cout << "[MyMaPIMapper::map] begin " << a.second.first << endl; //exit(1);
		vector<pair<int, pair<R, double> > > m;

		Solution<R> s(a.first);

		hmap->exec(s,a.second.second.first,a.second.second.second);

		Evaluation<M> e = eval.evaluate(s);

		R mapped = R(s.getR());

		m.push_back( make_pair(-1, make_pair( mapped , e.evaluation() ) ) );
		cout << "[MyMaPIMapper::map] end " << a.second << " " << m << endl;

		return m;
	}
};

template<class R, class M>
class MyMaPIReducer: public MaPI_Reducer<R, RankAndStop, int, pair<R, double> , R>
{
private:
	Evaluator<R, M>& eval;
	Heuristic<R, M>* hreduce;
public:
	MyMaPIReducer(MaPI_MapReduce<R, RankAndStop, int, pair<R, double> , R> * mr, MaPI_Serializer<R, RankAndStop, int, pair<R, double> , R> * s,Evaluator<R, M>& _eval) :
		MaPI_Reducer<R, RankAndStop, int, pair<R, double> , R> (mr,s),eval(_eval),hreduce(NULL)
	{
	}

	void setHeuristic(Heuristic<R, M>* h) {hreduce = h;}

	virtual pair<int, R> reduce(pair<int, vector<pair<R, double> > > bs)
	{
		//cout << "[MyMaPIReducer::reduce] begin " << bs.first << endl;

		//

		R reduced;

		if (hreduce == NULL) { // reduction best solution

			int bestIndex = 0;
			double bestCost = bs.second[bestIndex].second;

			for (unsigned i = 0 ; i < bs.second.size() ; i++)
			{
				if ( eval.betterThan(bs.second[i].second,bestCost) )
				{
					bestIndex = i;
					bestCost = bs.second[i].second;
				}
			}

			reduced = bs.second[bestIndex].first;/**/
		}
		else
		{
			// reduction based on a heuristic method

			if (bs.second.size() == 0) return pair<int, R> (bs.first, R());

			Population<R> pop;
			for (unsigned i = 0 ; i < bs.second.size() ; i++) pop.push_back( * new TestSolution<R>(bs.second[i].first) );

			//Population<R> & popReduced =
			hreduce->exec(pop, 60, 0); // TODO considerar tempo

			if (pop.size() == 0) return pair<int, R> (bs.first, R());
			if (pop.size() == 1) return pair<int, R> (bs.first, pop.at(0).getR());

			Evaluation<M>& ebest = eval.evaluate(pop.at(0));
			unsigned bestIndex = 0;

			for (unsigned i = 0 ; i < pop.size() ; i++) // TODO
			{
				Evaluation<M>& e = eval.evaluate(pop.at(i).getR());

				if ( eval.betterThan(e,ebest) )
				{
					bestIndex = i;
					ebest = e;
					delete &e;
				}
			}

			reduced = pop.at(bestIndex).getR();

			//for (unsigned i = 0 ; i < popReduced.size() ; i++) delete &popReduced.at(i);
		}

		//cout << "[MyMaPIReducer::reduce] end " << bs.first << endl;

		return pair<int, R> (bs.first, reduced);
	}
};

template<class R, class M = OPTFRAME_DEFAULT_MEMORY>
class OptFrameMapReduce: public Heuristic<R, M>
{
private:
	Evaluator<R, M>& evaluator;

	MyMaPISerializer<R, M> &serializer;
	MaPI_MapReduce<R, RankAndStop, int, pair<R, double> , R> &mapReduce;
	MyMaPIMapper<R, M> &mapper;
	MyMaPIReducer<R, M> &reducer;

public:

	using Heuristic<R, M>::exec; // prevents name hiding

	OptFrameMapReduce(
			MyMaPISerializer<R, M> &_serializer,
			MaPI_MapReduce<R, RankAndStop, int, pair<R, double> , R> &_mapReduce,
			MyMaPIMapper<R, M> &_mapper,
			MyMaPIReducer<R, M> &_reducer,
			Evaluator<R, M>& _eval) :
				serializer(_serializer),mapReduce(_mapReduce),mapper(_mapper),reducer(_reducer),
				evaluator(_eval)
	{
	}

	virtual void exec(Solution<R>& s, double timelimit, double target_f)
	{
		Evaluation<M>& e = evaluator.evaluate(s.getR());
		exec(s, e, timelimit, target_f);
		delete &e;
	}

	virtual void exec(Solution<R>& s, Evaluation<M>& e, double timelimit, double target_f)
	{
		vector< pair<R,RankAndStop> > input;
		for (int i = 0 ; i < mapReduce.getMPISize()-1 ; i++)
			input.push_back(make_pair(s.getR(),make_pair(i,make_pair(timelimit,target_f))));

		vector< pair<int,R> > output = mapReduce.run(mapper,reducer,input);

		Solution<R>& s1 = s.clone();
		if (output.size()>0)
			s1.setR(output[0].second);

		Evaluation<M>& e1 = evaluator.evaluate(s1);

		if(evaluator.betterThan(e1,e))
		{
			s = s1;
			e = e1;
			e.print();
		}

		delete &s1;
		delete &e1;
	}
};

#endif /*OptFrameMapReduce_HPP_*/
