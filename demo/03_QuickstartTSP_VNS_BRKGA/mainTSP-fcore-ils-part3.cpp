//
ILSLPerturbationLPlus2<ESolutionTSP> pert(eval, nsswap, rg2);

IteratedLocalSearchLevels<ESolutionTSP> ils(eval, initRand, VND, pert, 10, 5);
// ils.setVerbose();

std::cout << "will start ILS for 3 seconds" << std::endl;

auto status = ils.search(
  StopCriteria<ESolutionTSP::second_type>{ 3.0 }); // 3.0 seconds max
ESolutionTSP best = *status.best;
// best solution value
best.second.print();

std::cout << "FINISHED" << std::endl;
return 0;
}