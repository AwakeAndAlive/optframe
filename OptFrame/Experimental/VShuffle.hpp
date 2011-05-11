// OptFrame - Optimization Framework

// Copyright (C) 2009, 2010, 2011
// Igor Machado - Mario Henrique Perche - Pablo Munhoz - Sabir Ribas
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

#ifndef VShuffle_HPP_
#define VShuffle_HPP_

#include <math.h>
#include <vector>

#include "../Heuristic.hpp"
#include "../NS.hpp"
#include "../Evaluator.hpp"
#include "../Solution.hpp"

//#define VShuffle_DEBUG

#ifdef  METHOD_DEBUG
#define VShuffle_DEBUG
#endif


template<class R, class M>
class VShuffle : public Heuristic<R,M>
{
public:
	using Heuristic<R, M>::exec; // prevents name hiding

	virtual void exec(Solution<R>& s, double timelimit, double target_f)
	{
		cerr << "VShuffle exec("<<target_f<<","<<timelimit<<")" << endl;
		random_shuffle(s.getR().begin(),s.getR().end());
	}

	virtual void exec(Solution<R>& s, Evaluation<M>& e, double timelimit, double target_f)
	{
		cerr << "VShuffle exec("<<target_f<<","<<timelimit<<")" << endl;
		random_shuffle(s.getR().begin(),s.getR().end());
	}

private:
};

#endif
