// mainKP-fcore-ex.cpp
#include <iostream>

#include "KP-fcore.hpp" // implementação da mochila

// import everything on main()
using namespace std;
using namespace optframe;
using namespace scannerpp;
using namespace KP_fcore;

int
main()
{
   std::cout << "======== Carregando Problema ========" << std::endl;
   // semente pseudo-aleatória fixa em zero
   srand(time(NULL));

   // load data into problem context 'pTSP'
   Scanner scanner{ File{ "knapsack-example.txt" } };
   pKP.load(scanner);
   std::cout << "número de elementos na mochila:" << pKP.n << std::endl;

   std::cout << "======== Testa Construtivo Aleatório ========" << std::endl;
   // invoca método 'generateSolution' do FCore 'FConstructive' para construtivo aleatório
   std::vector<bool> sol = *construtivoAleatorio.generateSolution(0.0);
   // imprime solução inicial
   std::cout << sol << std::endl;
   //
   std::cout << "======== Testa Avaliador ========" << std::endl;
   // avalia solução inicial e cria um par 'ESolution'
   ESolutionKP esol(sol, avaliador.evaluate(sol));
   // imprime avaliação da solução inicial
   esol.second.print();

   std::cout << "======== Executa Simulated Annealing ========" << std::endl;
   // Especifica um gerador aleatório para o Simulated Annealing
   RandGen rg;
   //
   // Cria objeto da classe 'InitialSearch' (parecido com 'construtivoAleatório')
   BasicInitialSearch<ESolutionKP> initRand(construtivoAleatorio, avaliador);
   // Instancia um Simulated Annealing com alpha=98%, iterações na temp = 100, temperatura inicial = 99999
   BasicSimulatedAnnealing<ESolutionKP> sa{
      avaliador, initRand, nsFlip, 0.98, 100, 99999, rg
   };
   // executa o SA e coleta o 'status' de saída
   // passa um 'Criterio de Parada' por tempo (= 10 segundos)
   SearchStatus status = sa.search(StopCriteria<ESolutionKP::second_type>{ 10.0 }); // 10.0 seconds max
   // pega melhor solução do método SA
   ESolutionKP melhor = *sa.getBestSolution();
   std::cout << "======== Imprime melhor solução do SA ========" << std::endl;
   // imprime representação da melhor solução
   cout << melhor.first << endl;
   // imprime avaliação da melhor solução
   melhor.second.print();

   std::cout << "======== Fim da Execução ========" << std::endl;
   return 0;
}