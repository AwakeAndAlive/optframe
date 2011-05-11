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

#ifndef EtII_EVALUATOR_HPP_
#define EtII_EVALUATOR_HPP_

//#include <cmath>
#include <stdlib.h>
#include <math.h>

#include "../../../OptFrame/Evaluation.hpp"
#include "../../../OptFrame/Evaluator.hpp"

#include "Representation.h"
#include "Memory.h"
#include "Solution.h"
#include "Evaluation.h"

#include "ProblemInstance.hpp"

#define EPSILON_EtII 0.0001

int numEvs = 0;

class EtIIEvaluator: public Evaluator<RepEtII, MemEtII>
{
private:
	EtIIProblemInstance& pEtII;

public:

	using Evaluator<RepEtII, MemEtII>::evaluate;

	EtIIEvaluator(EtIIProblemInstance& _pEtII) : // If necessary, add more parameters
		pEtII(_pEtII)
	{
		// Put the rest of your code here
	}

	EvaluationEtII& evaluate(const RepEtII& rep)
	{
		//counting evaluations.
		numEvs++;
		//cout<<"##### Number of evaluates: "<<numEvs<<endl;

		int fo = 0; // Evaluation Function Value

		// horizontal
		for (int i = 0; i < rep.getRows(); i++)
			for (int j = 0; j < rep.getCols() - 1; j++)
				if (rep(i, j).right == rep(i, j + 1).left)
					fo++;

		// vertical
		for (int j = 0; j < rep.getCols(); j++)
			for (int i = 0; i < rep.getRows() - 1; i++)
				if (rep(i, j).down == rep(i + 1, j).up)
					fo++;

		int& mem = *new int;
		mem = 0;

		return *new EvaluationEtII(fo, mem);
	}

	/*
	virtual void evaluate(Evaluation<MemEtII>& e, const RepEtII&)
	{
		e.setObjFunction(e.getObjFunction() + e.getM());
		e.getM() = 0;
	}
	*/

	virtual bool betterThan(double a, double b)
	{
		// MAXIMIZATION
		return (a > (b + EPSILON_EtII));
	}

};

#endif /*EtII_EVALUATOR_HPP_*/

