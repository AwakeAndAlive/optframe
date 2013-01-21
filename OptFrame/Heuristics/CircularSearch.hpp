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

#ifndef OPTFRAME_CS_HPP_
#define OPTFRAME_CS_HPP_

#include "../LocalSearch.hpp"
#include "../NSSeq.hpp"
#include "../Evaluator.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class CircularSearch: public LocalSearch<R, ADS, DS>
{
private:
   Evaluator<R, ADS, DS>& eval;
   NSEnum<R, ADS, DS>& ns;

   int initial_w;

public:

	CircularSearch(Evaluator<R, ADS, DS>& _eval, NSEnum<R, ADS, DS>& _nsEnum) :
		eval(_eval), ns(_nsEnum)
	{
           initial_w = 0;
	}

	virtual ~CircularSearch()
	{
	}

	virtual void exec(Solution<R, ADS>& s, double timelimit, double target_f)
	{
		Evaluation<DS>& e = eval.evaluate(s.getR());
		exec(s, e, timelimit, target_f);
		delete &e;
	}

	virtual void exec(Solution<R, ADS>& s, Evaluation<DS>& e, double timelimit, double target_f)
	{
                int Wmax = ns.size();

                int w = initial_w % Wmax;

                do
                {
                    Move<R, ADS, DS>& m = ns.move(w);

                    if(m.canBeApplied(s))
                    {
                      double cost = eval.moveCost(e, m, s);

                      if(eval.betterThan(cost, 0))
                      {
                         //double old_f = e.evaluation();

                         delete &m.apply(e, s);
                         eval.evaluate(e, s); // updates 'e'

                         //cout << "CS improvement! w:" << w << " fo=" << e.evaluation() << " (antiga fo="<< old_f << ")" << endl << endl;

                         initial_w = w+1;

                         delete &m;
                         return;
                      }
                    }

                    delete &m;
                 
                    w = (w+1) % Wmax;
                 }
                 while(w != initial_w);
	}

	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (LocalSearch<R, ADS, DS>::compatible(s));
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearch<R, ADS, DS>::idComponent() << "CS";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};


template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class CircularSearchBuilder : public LocalSearchBuilder<R, ADS, DS>
{
public:
	virtual ~CircularSearchBuilder()
	{
	}

	virtual LocalSearch<R, ADS, DS>* build(Scanner& scanner, HeuristicFactory<R, ADS, DS>& hf, string family = "")
	{
		Evaluator<R, ADS, DS>* eval;
		hf.assign(eval, scanner.nextInt(), scanner.next()); // reads backwards!

		NSEnum<R, ADS, DS>* nsenum;
		hf.assign(nsenum, scanner.nextInt(), scanner.next()); // reads backwards!

		return new CircularSearch<R, ADS, DS>(*eval, *nsenum);
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		params.push_back(make_pair(Evaluator<R, ADS, DS>::idComponent(), "evaluation function"));
		params.push_back(make_pair(NSEnum<R, ADS, DS>::idComponent(), "neighborhood structure"));

		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == CircularSearch<R, ADS, DS>::idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearchBuilder<R, ADS, DS>::idComponent() << "CS";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};


#endif /*OPTFRAME_CS_HPP_*/
