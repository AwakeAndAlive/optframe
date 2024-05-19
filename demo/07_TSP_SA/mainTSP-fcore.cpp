

#include <iostream>
//
#include "TSP-fcore.hpp"
// implementation of TSP

// import everything on main()
using namespace std;
using namespace optframe;
using namespace scannerpp;
using namespace TSP_fcore;

int
main()
{
    srand(0); // using system random (weak... just an example!)

    // Load data from TSP file into problem context 'pTSP'
    std::ifstream infile("instances/00_firsttest.tsp");
    if (!infile)
    {
        std::cerr << "Error: Could not open TSP file." << std::endl;
        return 1;
    }

    Scanner scanner(&infile);
    pTSP.load(scanner);
    std::cout << pTSP.dist << std::endl;

   // evaluator
   // TSPEval ev;
   //
   // create simple solution
   // TSPRandom crand;
   //
   std::vector<int> sol = *crand.generateSolution(0);
   std::cout << sol << std::endl;

   // evaluation value and store on ESolution pair
   ESolutionTSP esol(sol, ev->evaluate(sol));
   esol.second.print(); // print evaluation

   // swap 0 with 1
   //MoveSwap move{ make_pair(0, 1), fApplySwap };

   MoveSwap move(0, 1);

   move.print();

   // NSSwap nsswap;
   // move for solution 'esol'
   auto m1 = nsswap.randomMove(esol);
   m1->print();

   std::cout << std::endl;
   std::cout << "begin listing NSSeqSwapFancy" << std::endl;
   //
   auto it1 = nsseq.getIterator(esol);
   for (it1->first(); !it1->isDone(); it1->next())
      it1->current()->print();
   std::cout << "end listing NSSeqSwapFancy" << std::endl;

   // Random number generator
   RandGen rg;                       // stack version
   sref<RandGen> rg2{ new RandGen }; // heap version (safely shared)
   // testing simulated annealing
   BasicInitialSearch<ESolutionTSP> initRand(crand, ev);

   BasicSimulatedAnnealing<ESolutionTSP> sa(
     //_evaluator, _constructive, _neighbors, _alpha, _SAmax, _Ti, _rg
     ev,
     initRand,
     nsseq,
     0.98,
     1000,
     99999,
     rg2);

   auto status = sa.search(
     StopCriteria<ESolutionTSP::second_type>{ 10.0 }); // 10.0 seconds max
   ESolutionTSP best = *status.best;                   //*sa.getBestSolution();
   // best solution value
   best.second.print();

   std::cout << "FINISHED" << std::endl;
   return 0;
}