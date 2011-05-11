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

// ===================================
// Main.cpp file generated by OptFrame
// Project Traveling Salesman Problem
// ===================================

#include <stdlib.h>
#include <math.h>

#include <iostream>

using namespace std;

#include <set>

#include "../OptFrame.hpp"
#include "TSP.h"

int main(int argc, char **argv)
{
	srand(time(NULL));
	RandGen rg(time(NULL));

	// Optimal value for berlin52 is 7542

	Scanner scanner(new File("./OptFrame/Examples/TSP/tsplib/berlin52.txt"));

	TSPProblemInstance* p = new TSPProblemInstance(scanner);

	RandomInitialSolutionTSP is(p,rg);

	SolutionTSP& s = is.generateSolution();

	NSEnumSwap ns(p,rg);

	s.print();

	TSPEvaluator eval(p);
	EvaluationTSP* e;

	e = &eval.evaluate(s);

	e->print();
	cout << endl;

	HeuristicFactory<RepTSP, MemTSP> factory(rg);
	factory.add_initsol(&is);
	factory.add_ev(&eval);
	factory.add_ns(&ns);


	OptFrame<RepTSP, MemTSP> optframe(factory);
	optframe.execute();

	cout << "Program ended successfully" << endl;

	return 0;
}
