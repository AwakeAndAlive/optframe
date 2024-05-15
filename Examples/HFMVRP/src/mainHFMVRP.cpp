// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2024 - OptFrame - https://github.com/optframe/optframe

// ===================================
// Main.cpp file generated by OptFrame
// Project HFMVRP
// ===================================

// C
#include <math.h>
#include <stdlib.h>

// C++
#include <iostream>

// optframe

#include <OptFrame/Helper/CloneConstructive.hpp>
#include <OptFrame/Heuristics/EA/ESContinuous.hpp>
#include <OptFrame/Heuristics/EA/NGES.hpp>
#include <OptFrame/Heuristics/Empty.hpp>
#include <OptFrame/Heuristics/GRASP/BasicGRASP.hpp>
#include <OptFrame/Heuristics/ILS/ILSLPerturbation.hpp>
#include <OptFrame/Heuristics/ILS/IteratedLocalSearchLevels.hpp>
#include <OptFrame/Heuristics/LocalSearches/BestImprovement.hpp>
#include <OptFrame/Heuristics/LocalSearches/FirstImprovement.hpp>
#include <OptFrame/Heuristics/LocalSearches/VariableNeighborhoodDescent.hpp>
#include <OptFrame/Heuristics/LocalSearches/VariableNeighborhoodDescentUpdateADS.hpp>
#include <OptFrame/NSSeq.hpp>
#include <OptFrame/Timer.hpp>
#include <OptFrame/Util/CheckCommand.hpp>
#include <OptFrame/Util/RandGenMersenneTwister.hpp>

//
#include <HFMVRP/HFMVRP.h>

using namespace std;

using namespace HFMVRP;

using namespace scannerpp;

int main(int argc, char** argv) {
  cout << argv[0] << endl;
  sref<RandGen> rg = new RandGenMersenneTwister;
  long seed = time(NULL);
  seed = 100;
  cout << "Seed = " << seed << endl;
  srand(seed);
  rg->setSeed(seed);

  Timer walltimer;

  // srand(seed);

  if (argc != 7) {
    cout << "Parametros incorretos! esperado 7 e recebeu " << argc << endl;
    cout << "Os parametros esperados sao: nome tempo fo_alvo mutationRate mi "
            "batch"
         << endl;
    exit(1);
  }

  const char* caminho = argv[1];
  double tempo = atof(argv[2]);
  double fo_alvo = atof(argv[3]);
  double mutationRate = atof(argv[4]);
  int mi = atoi(argv[5]);
  int batch = atoi(argv[6]);

  string nome = caminho;

  cout << "Parametros:" << endl;
  cout << "nome=" << nome << endl;
  cout << "tempo=" << tempo << endl;
  cout << "fo_alvo=" << fo_alvo << endl;
  cout << "mutationRate=" << mutationRate << endl;
  cout << "mi=" << mi << endl;
  cout << "batch=" << batch << endl;

  Scanner scanner{File(nome)};
  ProblemInstance* p = new ProblemInstance{scanner};

  // HFMVRPEvaluator* eval = new HFMVRPEvaluator{ *p };
  // sref<GeneralEvaluator<ESolutionHFMVRP, EvaluationHFMVRP>> eval = new
  // HFMVRPEvaluator{ *p };
  sref<Evaluator<SolutionHFMVRP, EvaluationHFMVRP, ESolutionHFMVRP>> eval =
      new HFMVRPEvaluator{*p};

  HFMVRPADSManager* adsMan = new HFMVRPADSManager{*p};
  double alpha = 0;
  ConstructiveSavingsRandomized is{*p, rg, *adsMan};

  sref<InitialSearch<ESolutionHFMVRP>> initSearch{
      new BasicInitialSearch<ESolutionHFMVRP>{
          new ConstructiveSavingsRandomized{*p, rg, *adsMan}, eval}};

  auto solTest = *is.generateSolution(0);
  ESolutionHFMVRP esolTest = {solTest, eval->evaluate(solTest)};

  //
  ESolutionHFMVRP esolTest2 =
      *initSearch->initialSearch({1.0}).first;  // 1.0 sec

  // sref<NSSeq<ESolutionHFMVRP, EvaluationHFMVRP>>
  // nsseq_deltaIterator_delta_2opt = new NSSeqVRP2Opt<int, AdsHFMVRP,
  // SolutionHFMVRP, DeltaMoveVRP2Opt, ProblemInstance,
  // DeltaNSIteratorVRP2Opt<DeltaMoveVRP2Opt>>(p);
  sref<NSSeq<ESolutionHFMVRP>> nsseq_deltaIterator_delta_2opt =
      new NSSeqVRP2Opt<ESolutionHFMVRP, ProblemInstance, DeltaMoveVRP2Opt,
                       DeltaNSIteratorVRP2Opt<DeltaMoveVRP2Opt>>(localGetRoutes,
                                                                 p);
  uptr<Move<ESolutionHFMVRP>> movetest =
      nsseq_deltaIterator_delta_2opt->randomMove(esolTest);
  movetest->print();

  // sref<NS<ESolutionHFMVRP, EvaluationHFMVRP>> ns_deltaIterator_delta_2opt =
  // new NSSeqVRP2Opt<int, AdsHFMVRP, SolutionHFMVRP, DeltaMoveVRP2Opt,
  // ProblemInstance, DeltaNSIteratorVRP2Opt<DeltaMoveVRP2Opt>>(p);
  sref<NS<ESolutionHFMVRP>> ns_deltaIterator_delta_2opt =
      nsseq_deltaIterator_delta_2opt;
  sref<NSSeq<ESolutionHFMVRP>> nsseq_deltaIterator_delta_or1 =
      new NSSeqVRPOrOpt1<ESolutionHFMVRP, ProblemInstance, DeltaMoveVRPOrOpt1,
                         DeltaNSIteratorVRPOrOpt1<DeltaMoveVRPOrOpt1>>(
          localGetRoutes, p);
  sref<NS<ESolutionHFMVRP>> ns_deltaIterator_delta_or1 =
      nsseq_deltaIterator_delta_or1;
  sref<NSSeq<ESolutionHFMVRP>> nsseq_deltaIterator_delta_or2 =
      new NSSeqVRPOrOpt2<ESolutionHFMVRP, ProblemInstance, DeltaMoveVRPOrOpt2,
                         DeltaNSIteratorVRPOrOpt2<DeltaMoveVRPOrOpt2>>(
          localGetRoutes, p);
  sref<NS<ESolutionHFMVRP>> ns_deltaIterator_delta_or2 =
      nsseq_deltaIterator_delta_or2;
  sref<NSSeq<ESolutionHFMVRP>> nsseq_deltaIterator_delta_exchange =
      new NSSeqVRPExchange<ESolutionHFMVRP, ProblemInstance,
                           DeltaMoveVRPExchange,
                           DeltaNSIteratorVRPExchange<DeltaMoveVRPExchange>>(
          localGetRoutes, p);
  sref<NS<ESolutionHFMVRP>> ns_deltaIterator_delta_exchange =
      nsseq_deltaIterator_delta_exchange;
  sref<NSSeq<ESolutionHFMVRP>> nsseq_delta_shift10 =
      new NSSeqVRPShift10<ESolutionHFMVRP, ProblemInstance, false,
                          DeltaIteratorVRPShift10>(p);
  sref<NS<ESolutionHFMVRP>> ns_delta_shift10 = nsseq_delta_shift10;
  sref<NSSeq<ESolutionHFMVRP>> nsseq_delta_swap11 =
      new NSSeqVRPSwap1_1<ESolutionHFMVRP, ProblemInstance, false,
                          DeltaIteratorVRPSwap1_1>(p);
  sref<NS<ESolutionHFMVRP>> ns_delta_swap11 = nsseq_delta_swap11;
  sref<NSSeq<ESolutionHFMVRP>> nsseq_deltaIterator_swap11 =
      new NSSeqVRPSwap1_1<ESolutionHFMVRP, ProblemInstance, false,
                          DeltaIteratorVRPSwap1_1>(p);
  sref<NS<ESolutionHFMVRP>> ns_deltaIterator_swap11 =
      nsseq_deltaIterator_swap11;
  sref<NSSeq<ESolutionHFMVRP>> nsseq_deltaIterator_shift10 =
      new NSSeqVRPShift10<ESolutionHFMVRP, ProblemInstance, false,
                          DeltaIteratorVRPShift10>(p);
  sref<NS<ESolutionHFMVRP>> ns_deltaIterator_shift10 =
      nsseq_deltaIterator_shift10;

  ILSLPerturbationLPlus2Prob<ESolutionHFMVRP>* ilsl_pert;

  // ilsl_pert = new ILSLPerturbationLPlus2Prob<ESolutionHFMVRP>(*eval, 100000,
  // *nsseq_deltaIterator_shift10, rg);
  //
  //
  //
  // ilsl_pert = new ILSLPerturbationLPlus2Prob<ESolutionHFMVRP>(*eval,
  // *nsseq_deltaIterator_shift10, rg);
  ilsl_pert = new ILSLPerturbationLPlus2Prob<ESolutionHFMVRP>(
      eval, ns_deltaIterator_shift10, rg);
  ilsl_pert->add_ns(ns_deltaIterator_swap11);
  ilsl_pert->add_ns(ns_deltaIterator_delta_2opt);
  ilsl_pert->add_ns(ns_deltaIterator_delta_or1);
  ilsl_pert->add_ns(ns_deltaIterator_delta_or2);
  ilsl_pert->add_ns(ns_deltaIterator_delta_exchange);
  int myints[] = {3, 3, 1, 1, 1, 1};
  vector<int> priorites(myints, myints + sizeof(myints) / sizeof(int));
  ilsl_pert->changeProb(priorites);

  // vector<LocalSearch<ESolutionHFMVRP>*> vLS;
  vsref<LocalSearch<ESolutionHFMVRP>> vLS;
  /*
   BestImprovement<ESolutionHFMVRP> fiI(eval, nsseq_deltaIterator_delta_2opt);
   BestImprovement<ESolutionHFMVRP> fiII(eval, nsseq_deltaIterator_delta_or1);
   BestImprovement<ESolutionHFMVRP> fiIII(eval, nsseq_deltaIterator_delta_or2);
   BestImprovement<ESolutionHFMVRP> fiIV(eval,
   nsseq_deltaIterator_delta_exchange); BestImprovement<ESolutionHFMVRP>
   fiV(eval, nsseq_deltaIterator_swap11); BestImprovement<ESolutionHFMVRP>
   fiVI(eval, nsseq_deltaIterator_shift10); vLS.push_back(&fiI);
   vLS.push_back(&fiII);
   vLS.push_back(&fiIII);
   vLS.push_back(&fiIV);
   vLS.push_back(&fiV);
   vLS.push_back(&fiVI);
   */

  vLS.push_back(new BestImprovement<ESolutionHFMVRP>(
      eval, nsseq_deltaIterator_delta_2opt));
  vLS.push_back(new BestImprovement<ESolutionHFMVRP>(
      eval, nsseq_deltaIterator_delta_or1));
  vLS.push_back(new BestImprovement<ESolutionHFMVRP>(
      eval, nsseq_deltaIterator_delta_or2));
  vLS.push_back(new BestImprovement<ESolutionHFMVRP>(
      eval, nsseq_deltaIterator_delta_exchange));
  vLS.push_back(
      new BestImprovement<ESolutionHFMVRP>(eval, nsseq_deltaIterator_swap11));
  vLS.push_back(
      new BestImprovement<ESolutionHFMVRP>(eval, nsseq_deltaIterator_shift10));

  //
  // CheckCommand<RepHFMVRP, AdsHFMVRP, SolutionHFMVRP> cc;
  //
  // template<XESolution XES, XEvaluation XEv = typename XES::second_type,
  // XRepresentation R = typename XES::first_type, class ADS = int,
  // XBaseSolution<R, ADS> S = CopySolution<R, ADS>, X2ESolution<XES> X2ES =
  // MultiESolution<XES>, XSearch<XES> XSH = std::pair<S, XEv>>
  //
  // CheckCommand<ESolutionHFMVRP, RepHFMVRP, AdsHFMVRP, SolutionHFMVRP> cc;
  CheckCommand<ESolutionHFMVRP, SolutionHFMVRP, RepHFMVRP, AdsHFMVRP> cc;
  //
  cc.add(eval);
  // cc.add(p);
  //
  // cc.add(is);
  cc.add(initSearch);
  //
  cc.add(nsseq_delta_shift10);
  cc.add(nsseq_delta_swap11);
  cc.add(nsseq_deltaIterator_swap11);
  cc.add(nsseq_deltaIterator_shift10);
  cc.add(nsseq_deltaIterator_delta_2opt);
  cc.add(nsseq_deltaIterator_delta_or1);
  cc.add(nsseq_deltaIterator_delta_or2);
  cc.add(nsseq_deltaIterator_delta_exchange);
  //
  cc.add(*adsMan);
  //
  cc.run(1, 1);
  getchar();
  getchar();

  // if randgen is given, it is RVND
  sref<GeneralEvaluator<ESolutionHFMVRP>> _ev = eval;
  vsref<LocalSearch<ESolutionHFMVRP>> _lsList = vLS;
  sptr<RandGen> _rg = rg.sptr();
  // VariableNeighborhoodDescent<ESolutionHFMVRP> newVND(eval, vLS, rg);
  VariableNeighborhoodDescent<ESolutionHFMVRP> newVND(eval, vLS, rg.sptr());
  // VariableNeighborhoodDescentUpdateADS<RepHFMVRP, AdsHFMVRP, SolutionHFMVRP>
  // newVNDUpdateADS(*eval, *adsMan, vLS);
  VariableNeighborhoodDescentUpdateADS<RepHFMVRP, AdsHFMVRP, SolutionHFMVRP>
      newVNDUpdateADS(eval, *adsMan, vLS);
  // BasicGRASP<SolutionHFMVRP, EvaluationHFMVRP, ESolutionHFMVRP>
  // basicGrasp(*eval, is, newVNDUpdateADS, alpha, 1000);
  BasicGRASP<ESolutionHFMVRP> basicGrasp(eval, is, newVNDUpdateADS, alpha,
                                         1000);

  EmptyLocalSearch<ESolutionHFMVRP> emptyLS;
  // vector<NSSeq<ESolutionHFMVRP>*> vNSeq;
  vsref<NSSeq<ESolutionHFMVRP>> vNSeq;
  vNSeq.push_back(nsseq_deltaIterator_delta_2opt);
  vNSeq.push_back(nsseq_deltaIterator_delta_or1);
  vNSeq.push_back(nsseq_deltaIterator_delta_or2);
  vNSeq.push_back(nsseq_deltaIterator_delta_exchange);
  vNSeq.push_back(nsseq_deltaIterator_swap11);
  vNSeq.push_back(nsseq_deltaIterator_shift10);

  StopCriteria<EvaluationHFMVRP> soscGR(10);  // target zero and 10 seconds
  soscGR.target_f = Evaluation<>(0);
  auto ret_grasp = basicGrasp.search(soscGR);
  std::optional<pair<SolutionHFMVRP, Evaluation<>>> initialPairGrasp =
      ret_grasp.best;  // basicGrasp.best;

  //
  // CloneConstructive<SolutionHFMVRP>
  // cloneSolAsConstructive(initialPairGrasp->first);
  //

  sref<InitialSearch<ESolutionHFMVRP>> initConstructive =
      new BasicInitialSearch<ESolutionHFMVRP>{
          new CloneConstructive<SolutionHFMVRP>{initialPairGrasp->first}, eval};

  vector<int> vNSeqMaxApplication(vNSeq.size(), 1000);
  int selectionType = 1;
  //	double mutationRate = 1;
  //	int mi = 5;
  // int batach = 5;

  // which ES?
  //===============
  //

  // ES<ESolutionHFMVRP> es(*eval, cloneSolAsConstructive, vNSeq,
  // vNSeqMaxApplication, emptyLS, selectionType, mutationRate, rg, mi, 6 * mi,
  // 50000, "./esOutput", batch); es.setMessageLevel(4);

  // double mutationRate = 1;
  // int selectionType = 1;

  string outputFile = "./esOutput";
  // NGESParams* ngesParams =
  //   new NGESParams(
  NGESParams ngesParams(vNSeqMaxApplication, selectionType, mutationRate, mi,
                        6 * mi, 50000, outputFile, batch);

  // NGES(sref<GeneralEvaluator<XES>> _eval, sref<InitialSearch<XES>>
  // _constructive, vsref<NS<XES, XSH>> _vNS, sref<LocalSearch<XES>> _ls,
  // sref<RandGen> _rg, NGESParams _ngesParams)
  //
  sref<GeneralEvaluator<ESolutionHFMVRP>> _eval = eval;
  sref<InitialSearch<ESolutionHFMVRP>> _constructive = initConstructive;
  vsref<NS<ESolutionHFMVRP>> _vNS;
  sref<LocalSearch<ESolutionHFMVRP>> _ls = emptyLS;
  // sref<RandGen> _rg = rg;

  // NGES<ESolutionHFMVRP>* es = new NGES<ESolutionHFMVRP>(*eval,
  // cloneSolAsConstructive, vNSeq, emptyLS, rg, *ngesParams);
  NGES<ESolutionHFMVRP>* es = new NGES<ESolutionHFMVRP>(
      _eval, _constructive, _vNS, _ls, rg, ngesParams);
  es->setMessageLevel(LogLevel::Info);

  //=====================

  // StopCriteria<ESolutionHFMVRP> soscES(180, 0);
  StopCriteria<EvaluationHFMVRP> soscES({180});
  // pair<Solution<ESolutionHFMVRP>, Evaluation<>>* initialSol =
  // es.search(soscES);
  auto initialOutput = es->search(soscES);
  ESolutionHFMVRP initialSol = *initialOutput.best;
  double objFuncES = initialSol.second.getObjFunction();
  cout << "getObjFunction: " << objFuncES << endl;

  //
  // =============

  // double objFuncES = 0; // which is this Vitor?

  size_t found = nome.find_last_of("/\\");
  string instanceName = nome.substr(found + 1);
  FILE* arquivo = fopen("./MyProjects/Results/log.txt", "a");
  if (!arquivo) {
    cout << "ERRO: falha ao criar arquivo \"./Results/log.txt\"" << endl;
  } else {
    fprintf(arquivo, "%s\t%f\t%f\t%d\t%d\t%ld\n", instanceName.c_str(),
            mutationRate, objFuncES, mi, batch, seed);
    fclose(arquivo);
  }

  /*	// ==================== BEGIN ILS AND GRASP ===========================
         //pair<Solution<ESolutionHFMVRP>&, Evaluation<>&>* initialPair =
   basicGrasp.search(30, 0);

         CloneConstructive<ESolutionHFMVRP>
   cloneSolAsConstructive(initialSol->first);
         IteratedLocalSearchLevels<ESolutionHFMVRP> ilsl(*eval,
   cloneSolAsConstructive, newVNDUpdateADS, *ilsl_pert, 130, 15);
         ilsl.setMessageLevel(4);

   StopCriteria<ESolutionHFMVRP> soscILSL(60,0);
         pair<Solution<ESolutionHFMVRP>, Evaluation<>>* pairILS =
   ilsl.search(soscILSL); Solution<ESolutionHFMVRP> ilsSOL = pairILS->first;

         EvaluationHFMVRP value = eval->evaluate(ilsSOL.getR());
         cout << "getObjFunction: " << value.getObjFunction() << endl;
         cout << "getInfMeasure: " << value.getInfMeasure() << endl;
         value.print();

         eval->exportEXCEL(ilsSOL.getR(), "./MyProjects/Results/teste");*/

  cout << "Program ended successfully" << endl;

  return 0;
}
