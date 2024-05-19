#include <iostream>
#include <vector>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <algorithm>
#include <chrono>
#include <fstream>
#include <sstream>

using namespace std;

//armazenar a solução
struct TSPSolution {
    vector<int> path;
    double cost;
};

//calcular a distância euclidiana
double euclideanDistance(const pair<int, int>& a, const pair<int, int>& b) {
    return sqrt(pow(a.first - b.first, 2) + pow(a.second - b.second, 2));
}

//calcular o custo total
double calculateTotalCost(const vector<pair<int, int>>& coordinates, const vector<int>& path) {
    double totalCost = 0.0;
    for (size_t i = 0; i < path.size() - 1; ++i) {
        totalCost += euclideanDistance(coordinates[path[i]], coordinates[path[i + 1]]);
    }
    totalCost += euclideanDistance(coordinates[path.back()], coordinates[path.front()]);
    return totalCost;
}

//gerar uma solução inicial aleatória
vector<int> generateInitialSolution(int n) {
    vector<int> path(n);
    for (int i = 0; i < n; ++i) {
        path[i] = i;
    }
    random_shuffle(path.begin(), path.end());
    return path;
}

//gerar uma vizinhança (trocando dois elementos de posição)
vector<int> generateNeighbor(const vector<int>& path) {
    vector<int> newPath = path;
    int i = rand() % newPath.size();
    int j = rand() % newPath.size();
    swap(newPath[i], newPath[j]);
    return newPath;
}

//função de resfriamento
double exponentialCooling(double initialTemp, int iteration, double coolingRate) {
    return initialTemp * pow(coolingRate, iteration);
}

//TSP com SA
TSPSolution simulatedAnnealingTSP(const vector<pair<int, int>>& coordinates, double initialTemp, double finalTemp, int maxIterations, double coolingRate) {
    srand(time(0));

    int n = coordinates.size();
    vector<int> currentSolution = generateInitialSolution(n);
    double currentCost = calculateTotalCost(coordinates, currentSolution);

    vector<int> bestSolution = currentSolution;
    double bestCost = currentCost;

    for (int iter = 0; iter < maxIterations; ++iter) {
        double temp = exponentialCooling(initialTemp, iter, coolingRate);
        if (temp < finalTemp) break;

        vector<int> newSolution = generateNeighbor(currentSolution);
        double newCost = calculateTotalCost(coordinates, newSolution);

        if (newCost < currentCost || exp((currentCost - newCost) / temp) > static_cast<double>(rand()) / RAND_MAX) {
            currentSolution = newSolution;
            currentCost = newCost;
        }

        if (newCost < bestCost) {
            bestSolution = newSolution;
            bestCost = newCost;
            cout << "Best fo: " << bestCost << " Found on Iter = " << iter << " and T = " << temp << ";" << endl;
        }
    }

    return { bestSolution, bestCost };
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <instance-file>" << endl;
        return 1;
    }

    string instanceFilePath = argv[1];
    string instanceName = instanceFilePath.substr(instanceFilePath.find_last_of("/\\") + 1);

    //carregar instância
    ifstream infile(instanceFilePath);
    if (!infile) {
        cerr << "Error: Could not open TSP file." << endl;
        return 1;
    }

    vector<pair<int, int>> coordinates;
    string line;
    int index;
    double x, y;

    //primeira linha do arquivo (número de cidades)
    if (!getline(infile, line)) {
        cerr << "Error: Could not read number of cities." << endl;
        return 1;
    }

    //coordenadas
    while (infile >> index >> x >> y) {
        coordinates.emplace_back(x, y);
    }

    if (coordinates.empty()) {
        cerr << "Error: No coordinates loaded from TSP file." << endl;
        return 1;
    }

    double initialTemp = 100000.0;
    double finalTemp = 0.1;
    int maxIterations = 20000;
    double coolingRate = 0.98;

    auto start = chrono::high_resolution_clock::now();
    
    //salvar a solução inicial para comparação 
    vector<int> initialSolution = generateInitialSolution(coordinates.size());
    double initialCost = calculateTotalCost(coordinates, initialSolution);

    TSPSolution bestSolution = simulatedAnnealingTSP(coordinates, initialTemp, finalTemp, maxIterations, coolingRate);
    auto end = chrono::high_resolution_clock::now();

    auto duration = chrono::duration_cast<chrono::microseconds>(end - start).count();

    double finalCost = bestSolution.cost;
    double improvement = ((initialCost - finalCost) / initialCost) * 100;

    //saídas
    cout << "Instance name: " << instanceName << ";" << endl;
    cout << "SA variables: initialTemp = " << initialTemp << "; finalTemp = " << finalTemp << "; maxIterations = " << maxIterations << "; coolingRate = " << coolingRate << ";" << endl;
    cout << "First solution evaluation: Evaluation function value = " << initialCost << ";" << endl;
    cout << "First solution: vector(" << initialSolution.size() << ") [";
    for (size_t i = 0; i < initialSolution.size(); ++i) {
        cout << initialSolution[i];
        if (i != initialSolution.size() - 1) {
            cout << " , ";
        }
    }
    cout << "];" << endl;
    cout << "Best solution evaluation: Evaluation function value = " << bestSolution.cost << ";" << endl;
    cout << "Final solution: vector(" << bestSolution.path.size() << ") [";
    for (size_t i = 0; i < bestSolution.path.size(); ++i) {
        cout << bestSolution.path[i];
        if (i != bestSolution.path.size() - 1) {
            cout << " , ";
        }
    }
    cout << "];" << endl;
    cout << "Improvement: " << improvement << "%;" << endl;
    cout << "Execution time: " << duration << " microseconds;" << endl;

    return 0;
}
