//Rename to mainTSP.cpp to run the example

// ===================================
// Main.cpp file generated by OptFrame
// Project Traveling Salesman Problem
// ===================================

#define MaPI

#include <stdlib.h>
#include <math.h>

#include <iostream>

using namespace std;

#include <set>

#include "../OptFrame/OptFrame.hpp"
#include "../OptFrame/Examples/TSP.h"

int main(int argc, char **argv)
{
	srand(time(NULL));

	// Optimal value for berlin52 is 7542

	Scanner scanner(new File("../OptFrame/Examples/TSP/tsplib/mona-lisa-benchmark.txt"));

	TSPProblemInstance* p = new TSPProblemInstance(scanner);

	RandomInitialSolutionTSP is(p);

	SolutionTSP& s = is.generateSolution();

	NSEnumSwap ns(p);

	s.print();

	TSPEvaluator eval(p);
	EvaluationTSP* e;

	e = &eval.evaluate(s);

	e->print();
	cout << endl;


	MaPI_MapReduce<RepTSP, int, int, pair<RepTSP, double> , RepTSP> mapReduce;
	MyMaPISerializer<RepTSP,MemTSP> serializer;
	MyMaPIMapper<RepTSP, MemTSP> mapper(&mapReduce, &serializer, eval, ns, argc >= 3 ? atoi(argv[2]) : 2); //TODO
	MyMaPIReducer<RepTSP, MemTSP> reducer(&mapReduce, &serializer, eval);


	HeuristicFactory<RepTSP, MemTSP> factory;
	factory.add_initsol(&is);
	factory.add_ev(&eval);
	factory.add_ns(&ns);

	factory.setMapReduce(&serializer,&mapReduce,&mapper,&reducer);


	OptFrame<RepTSP, MemTSP> optframe(&factory);
	//optframe.execute();


	mapReduce.initServers(argc,argv);

	Timer timer;

	string cmd("read ");
	if (argc > 1) cmd += argv[1]; else cmd += "exampleMaPI.opt";
	optframe.execute(string("read ") + argv[1]);

	cout << "Program ended successfully" << endl;

	return 0;
}
