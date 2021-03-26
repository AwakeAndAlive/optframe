
#include<iostream>
#include "TSP-fcore.hpp" // implementation of TSP

// import everything on main()
using namespace std;
using namespace optframe;
using namespace scannerpp;
using namespace TSP_fcore; 

int
main()
{
   srand(0); // using system random (weak... just an example!)

   // load data into problem context 'pTSP'
   Scanner scanner{ "3\n1 10 10\n2 20 20\n3 30 30\n" };
   pTSP.load(scanner);
   std::cout << pTSP.dist << std::endl;

   // evaluator
   //TSPEval ev;
   //
   // create simple solution
   //TSPRandom crand;
   //
   std::vector<int> sol = *crand.generateSolution(0);
   std::cout << sol << std::endl;

   // evaluation value and store on ESolution pair
   ESolutionTSP esol(sol, ev.evaluate(sol));
   esol.second.print(); // print evaluation

   // swap 0 with 1
   MoveSwap move{ make_pair(0, 1), fApplySwap };
   move.print();

   //NSSwap nsswap;
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
   RandGen rg;
   sref<RandGen> rg2 { new RandGen };
   // testing simulated annealing
   BasicInitialSearch<ESolutionTSP> initRand(crand, ev);

   sref<InitialSearch<ESolutionTSP>> initRand2{
      new BasicInitialSearch<ESolutionTSP>(crand, ev)
   };

   //BasicSimulatedAnnealing<ESolutionTSP> sa{
   //   ev, initRand, nsseq, 0.98, 100, 99999, rg
   //};

   static_assert(!std::is_aggregate_v<BasicSimulatedAnnealing<ESolutionTSP>>); 
   //static_assert( std::is_convertible_v<NS<ESolutionTSP>*, NSSeq<ESolutionTSP>*>); 
   //
   // only with aggregates
   /*
   BasicSimulatedAnnealing<ESolutionTSP> sa = {
      .evaluator{ev2},
      .constructive{initRand2},
      .alpha{0.98},
      .SAMax{100}, 
      .Ti{99999},
      .rg{rg2}
   };
   */

  sref<GeneralEvaluator<ESolutionTSP>> _evaluator = ev2;
  sref<InitialSearch<ESolutionTSP>> _constructive = initRand2;
  sref<NS<ESolutionTSP>> _neighbors = nsseq2;
  //sref<NSSeq<ESolutionTSP>> _neighbors2 = (sref<NSSeq<ESolutionTSP>>)_neighbors;
  double _alpha = 0.98;
  int _SAmax = 100;
  double _Ti = 99999;
  sref<RandGen> _rg = rg2;
   
   //BasicSimulatedAnnealing<ESolutionTSP> sa(
   //   ev2, initRand2, nsseq2, 0.98, 100, 99999, rg2
   //);
   
   BasicSimulatedAnnealing<ESolutionTSP> sa(
      _evaluator, _constructive, _neighbors, _alpha, _SAmax, _Ti, _rg
   );
   

   auto status = sa.search(StopCriteria<ESolutionTSP::second_type>{ 10.0 }); // 10.0 seconds max
   ESolutionTSP best = *status.best;//*sa.getBestSolution();
   // best solution value
   best.second.print();

   std::cout << "FINISHED" << std::endl;
   return 0;
}