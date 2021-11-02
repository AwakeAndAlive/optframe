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

// ===================================
// Main.cpp file generated by OptFrame
// Project Traveling Salesman Problem
// ===================================

// mainTSP.cpp

#include <math.h>
#include <stdlib.h>

#include <filesystem>
#include <iostream>

using namespace std;

#include <set>

//#include <OptFrame/BaseConcepts.ctest.hpp> // tsting concepts
#include <OptFrame/Evaluation.hpp>
#include <OptFrame/Solution.hpp>
#include <OptFrame/printable/printable.hpp>
//#include "../OptFrame/Util/TestSolution.hpp"

#include <OptFrame/Heuristics/NSearch/FirstImprovingNeighbor.hpp>

#include <OptFrame/Heuristics/EvolutionaryAlgorithms/BRKGA.hpp>
#include <OptFrame/Loader.hpp>
////#include "../OptFrame/Util/BuildCommand.hpp"  // TODO: return after Concepts OptFrame v4
#include "TSP.h"
#include <OptFrame/Util/CheckCommand.hpp>

////#include <OptFrame/EvaluatorAdapter.hpp>

using namespace TSP;
using namespace scannerpp;

//#include "../OptFrame/Util/PackTypes.hpp" // deprecated!!

// gets real file path, if file exists, or empty string, if not
std::string
resolvePath(std::string sinstance, std::string appPath, std::string bazelPackage = "")
{
   // try relative path
   if (std::filesystem::exists(sinstance))
      return sinstance;

   // load file from absolute directory that contains executable
   std::filesystem::path exec = appPath;
   std::string full_instance = exec.parent_path().string() + std::string("/") + sinstance;
   //std::cout << "loading instance at '" << full_instance << "'" << std::endl;
   if (std::filesystem::exists(full_instance))
      return full_instance;

   // try .runfiles directory extension (bazel build for external bazel package)
   full_instance = exec.string() + std::string(".runfiles/") + std::string(bazelPackage) + std::string("/") + sinstance;
   if (std::filesystem::exists(full_instance))
      return full_instance;

   // instance not found
   return "";
}

int
main(int argc, char** argv)
{
   // ADS still exists, only because of ADSManager...
   Loader<RepTSP, OPTFRAME_DEFAULT_ADS, SolutionTSP, EvaluationTSP, ESolutionTSP> optframe;
   TSPProblemCommand tsp;

   // instance relative path to executable directory
   std::string sinstance = "tsplib/berlin52.txt";

   std::string good_path = resolvePath(sinstance, argv[0], "TSP");

   if (good_path == "") {
      std::cerr << "Instances not found in executable directory. Aborting." << std::endl;
      return 1; // cannot open file
   }

   File file(good_path);
   //File file("./tsplib/berlin52.txt");

   if (!file.isOpen()) {
      cout << "File not found" << endl;
      return 1;
   }

   Scanner scanner(std::move(file));
   tsp.load(scanner, optframe.factory, optframe.dictionary, optframe.ldictionary);

   /*
	FILE* outf = fopen("berlin52.mtx","w");
	fprintf(outf, "%d\n", tsp.p->n);
	for(unsigned i=0; i<tsp.p->n; i++) {
		for(unsigned j=0; j<tsp.p->n; j++)
			fprintf(outf, "%d\t",int(100*(*tsp.p->dist)(i,j)));
		fprintf(outf, "\n");
	}

	fclose(outf);
	exit(1);
	*/

   bool check_verbose = false;
   //
   //CheckCommand<RepTSP, OPTFRAME_DEFAULT_ADS, SolutionTSP> check(check_verbose);
   //
   //
   //CheckCommand<ESolutionTSP, RepTSP, OPTFRAME_DEFAULT_ADS, SolutionTSP> check(check_verbose);
   //
   CheckCommand<ESolutionTSP> check(check_verbose);

   RandGenMersenneTwister rg(0);
   sref<RandGen> rg2 = RandGenMersenneTwister(0);
   //
   TSPEvaluator eval1(tsp.p); // Should not be Specific to TSP!! Won't work on Decoder..
   sref<Evaluator<SolutionTSP, EvaluationTSP>> eval = eval1;

   RandomInitialSolutionTSP randomTSP(tsp.p, eval, rg);
   NearestNeighborConstructive cnn(tsp.p, eval, rg);
   ConstructiveBestInsertion cbi(tsp.p, eval, rg);

   // NSEnumSwap enumswap(tsp.p, rg);
   sref<NSEnum<ESolutionTSP>> enumswap{
      new NSEnumSwap(tsp.p, rg)
   };

   // Basic test for Neighborhood Exploration
   FirstImprovingNeighbor<ESolutionTSP> fin(eval, enumswap);
   // =======================================

   /*
   NSSeqTSP2Opt<int, OPTFRAME_DEFAULT_ADS, SolutionTSP, DeltaMoveTSP2Opt, ProblemInstance> nsseq_delta_2opt(tsp.p);
   NSSeqTSP2Opt<int, OPTFRAME_DEFAULT_ADS, SolutionTSP> tsp2opt;
   NSSeqTSPOrOptk<int, OPTFRAME_DEFAULT_ADS, SolutionTSP, DeltaMoveTSPOrOptk, ProblemInstance> nsseq_delta_or1(1, tsp.p);
   NSSeqTSPOrOptk<int, OPTFRAME_DEFAULT_ADS, SolutionTSP> tspor1(1);
   NSSeqTSPOrOptk<int, OPTFRAME_DEFAULT_ADS, SolutionTSP> tspor2(2);
   NSSeqTSPOrOptk<int, OPTFRAME_DEFAULT_ADS, SolutionTSP> tspor3(3);
*/
   sref<NSSeq<ESolutionTSP>> nsseq_delta_2opt = new NSSeqTSP2Opt<int, OPTFRAME_DEFAULT_ADS, SolutionTSP, DeltaMoveTSP2Opt, ProblemInstance>(tsp.p);
   sref<NSSeq<ESolutionTSP>> tsp2opt = new NSSeqTSP2Opt<int, OPTFRAME_DEFAULT_ADS, SolutionTSP>();
   sref<NSSeq<ESolutionTSP>> nsseq_delta_or1 = new NSSeqTSPOrOptk<int, OPTFRAME_DEFAULT_ADS, SolutionTSP, DeltaMoveTSPOrOptk, ProblemInstance>(1, tsp.p);
   sref<NSSeq<ESolutionTSP>> tspor1 = new NSSeqTSPOrOptk<int, OPTFRAME_DEFAULT_ADS, SolutionTSP>(1);
   sref<NSSeq<ESolutionTSP>> tspor2 = new NSSeqTSPOrOptk<int, OPTFRAME_DEFAULT_ADS, SolutionTSP>(2);
   sref<NSSeq<ESolutionTSP>> tspor3 = new NSSeqTSPOrOptk<int, OPTFRAME_DEFAULT_ADS, SolutionTSP>(3);

   // TODO: we need to try NSSeqTSPOrOpt , because it requires adapters...
   NSSeqTSPOrOpt<int, OPTFRAME_DEFAULT_ADS, SolutionTSP> tspor_adapt;
   // Maybe S& should be the Representation itself over there.... no getR() inside there.
   // It makes more sense to pass RepTSP + ESolutionTSP... than SolutionTSP + ESolutionTSP
   // Then, should adapters just work for R,ADS pair on XBaseSolution concept?? TODO: think...

   sref<NSSeq<ESolutionTSP>> tspswap = new NSSeqTSPSwap<int, OPTFRAME_DEFAULT_ADS, SolutionTSP>();

   check.add(randomTSP);
   check.add(cnn);
   check.add(cbi);
   check.add(eval);
   check.add(enumswap);
   check.add(nsseq_delta_2opt);
   check.add(tsp2opt);
   check.add(nsseq_delta_or1);
   check.add(tspor1);
   check.add(tspor2);
   check.add(tspor3);
   check.add(tspswap);

   //check.run(100, 10);

   cout << "will test BRKGA (n=" << tsp.p->n << ")" << endl;

   TSPRepEvaluator eval_rep(tsp.p);

   //Evaluator<SolutionTSP>& eval2 = eval;
   EvaluatorPermutationRandomKeys<EvaluationTSP, double> eprk(eval_rep, 0, tsp.p->n - 1);

   // BRKGA is using Representation instead of Solution... beware!
   sref<InitialPopulation<pair<std::vector<double>, EvaluationTSP>>> _initPop =
     new RandomKeysInitPop<EvaluationTSP, double>(tsp.p->n, rg2);
   BRKGA<pair<RepTSP, EvaluationTSP>, double> brkga(
     eprk,
     _initPop, // key_size = tsp.p->n
     10000,
     10,
     0.4,
     0.3,
     0.6,
     rg2);

   StopCriteria<EvaluationTSP> sosc;
   // strange that this worked.... it's against 'override' pattern. Very strange!!
   /*
   pair<CopySolution<random_keys>, Evaluation<>>* r2 = brkga.search(sosc);
   r2->first.print();
   pair<Evaluation<>, CopySolution<vector<int>>*> pd = eprk.decode(r2->first.getR());
   pd.second->print();
   if (eval.verify(pd.second->getR()))
      cout << "CHECK: OK" << endl;
   pd.first.print();
   */

   //pair<SolutionTSP, Evaluation<>>* r2 = brkga.search(sosc);

   auto sout = brkga.search(sosc.start());
   //std::optional<ESolutionTSP> r2 = brkga.best;
   auto r2 = sout.best;
   //virtual std::optional<pair<XRS, XEv>> search(StopCriteria<XEv>& stopCriteria, const std::optional<pair<XRS, XEv>> input)

   //r2->first.print();
   std::cout << r2->first << std::endl;

   r2->second.print();

   cout << "end BRKGA tests" << endl;

   // TODO: return after refactor on Concepts and OptFrame v4
   /*
   BuildCommand<RepTSP, OPTFRAME_DEFAULT_ADS, SolutionTSP> build;
   for (unsigned i = 0; i <= 7; i++) {
      stringstream ss;
      ss << "OptFrame:ComponentBuilder:LocalSearch:BI  OptFrame:Evaluator 0  OptFrame:NS:NSSeq " << i;
      string name = build.run(optframe.factory, optframe.dictionary, optframe.ldictionary, ss.str());
      cout << "BUILT: '" << name << "'" << endl;
   }
   */

   vsref<LocalSearch<ESolutionTSP>> ns_list;
   ns_list.push_back(new BestImprovement<ESolutionTSP>(eval, tsp2opt));
   ns_list.push_back(new BestImprovement<ESolutionTSP>(eval, tspor1));
   ns_list.push_back(new BestImprovement<ESolutionTSP>(eval, tspor2));
   ns_list.push_back(new BestImprovement<ESolutionTSP>(eval, tspor3));
   ns_list.push_back(new BestImprovement<ESolutionTSP>(eval, tspswap));
   for (unsigned i = 0; i < ns_list.size(); i++)
      ns_list[i]->setVerbose();

   VariableNeighborhoodDescent<ESolutionTSP> VND(eval, ns_list);
   VND.setVerbose();

   ILSLPerturbationLPlus2<ESolutionTSP> pert(eval, tsp2opt, rg);
   pert.add_ns(tspor1);
   pert.add_ns(tspor2);
   pert.add_ns(tspor3);
   pert.add_ns(tspswap);

   IteratedLocalSearchLevels<ESolutionTSP> ils(eval, randomTSP, VND, pert, 3, 2);
   //ils.setMessageLevel(4);
   ils.setVerbose();
   if (ils.information)
      cout << "infomation is on for ILS" << endl;

   cout << "will run ils" << endl;
   Timer tim;
   StopCriteria<EvaluationTSP> soscILS;
   soscILS.timelimit = 3; // 1000
   soscILS.target_f = EvaluationTSP(0.0);
   //pair<CopySolution<RepTSP>, Evaluation<>>& psol = *ils.search(soscILS, NULL, NULL);
   auto sout2 = ils.search(soscILS);
   std::optional<ESolutionTSP> psol = sout2.best;
   cout << "finished ILS!" << endl;
   cout << tim.now() << " secs" << endl;

   psol->first.print();
   psol->second.print();
   cout << endl
        << endl;

   // ===========

   //for (unsigned i = 0; i < ns_list.size(); i++)
   //   delete ns_list[i];

   vsref<NS<ESolutionTSP>> v_ns;
   //vsref<NSSeq<ESolutionTSP>> v_nsseq = { tsp2opt, tspor1, tspor2, tspor3, tspswap };
   vsref<NSSeq<ESolutionTSP>> v_nsseq;
   v_nsseq.push_back(tsp2opt);
   v_nsseq.push_back(tspor1);
   v_nsseq.push_back(tspor2);
   v_nsseq.push_back(tspor3);
   v_nsseq.push_back(tspswap);

   //v_nsseq.push_back(&tsp2opt);
   //v_nsseq.push_back(&tspor1);
   //v_nsseq.push_back(&tspor2);
   //v_nsseq.push_back(&tspor3);
   //v_nsseq.push_back(&tspswap);
   for (unsigned i = 0; i < v_nsseq.size(); i++)
      v_ns.push_back(v_nsseq[i]);

   BasicVNS<ESolutionTSP> vns(eval, randomTSP, v_ns, v_nsseq);
   vns.setMessageLevel(LogLevel::Info); // INFORMATION
   StopCriteria<EvaluationTSP> soscVNS;
   soscVNS.timelimit = 2; // 2 seconds
   soscVNS.target_f = EvaluationTSP(7550.0);

   // zero is better than any positive value
   assert(eval->betterThan(EvaluationTSP(0), soscVNS.target_f));

   //pair<CopySolution<RepTSP>, Evaluation<>>& psol2 = *vns.search(sosc, NULL, NULL);
   auto sout3 = vns.search(soscVNS.start());
   std::optional<ESolutionTSP> psol2 = sout3.best;
   psol2->first.print();
   psol2->second.print();

   // Remember the old times...
   /*
	 echo building VND
	 define vnd_list [ OptFrame:LocalSearch: 0 ,  OptFrame:LocalSearch: 1, OptFrame:LocalSearch: 2, OptFrame:LocalSearch: 3 ]
	 component.create_list $vnd_list OptFrame:LocalSearch: comp_vnd_list
	 build OptFrame:LocalSearch:VND   $Evaluator 0   $comp_vnd_list   vnd

	 %component.list

	 echo building ILS
	 build OptFrame:ComponentBuilder:SingleObjSearch:ILS:ILSLevels   $Evaluator 0    $Constructive 0    $vnd   OptFrame:ILS:LevelPert:LPlus2 0    100    8  meu_ils

	 test 2 3 7000 7000   $Evaluator 0   $meu_ils   output.txt   solucao_saida

	 evaluate $Evaluator 0 $solucao_saida
	 */

   cout << "Program ended successfully" << endl;

   return 0;
}
