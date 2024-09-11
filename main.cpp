#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <random>

using namespace std;

// First-Fit (Greedy) Algorithm
int firstFit(vector<int>& weights, int binCapacity) {
    vector<int> bins;

    for (int weight : weights) {
        bool placed = false;

        for (int& bin : bins) {
            if (bin + weight <= binCapacity) {
                bin += weight;
                placed = true;
                break;
            }
        }

        if (!placed) {
            bins.push_back(weight);
        }
    }

    return bins.size();
}

// Reinsertion neighborhood
bool reinsertion(vector<int>& weights, int binCapacity, int& bestBins) {
    bool improvement = false;
    for (int i = 0; i < weights.size(); i++) {
        for (int j = 0; j < weights.size(); j++) {
            if (i != j) {
                // Reinserting item i at position j
                int temp = weights[i];
                weights.erase(weights.begin() + i);
                weights.insert(weights.begin() + j, temp);
                int newBins = firstFit(weights, binCapacity);
                if (newBins < bestBins) {
                    bestBins = newBins;
                    improvement = true;
                } else {
                    // Undo reinsertion if no improvement
                    weights.erase(weights.begin() + j);
                    weights.insert(weights.begin() + i, temp);
                }
            }
        }
    }
    return improvement;
}

// Two-opt neighborhood: Swap between two bins
bool twoOptSwap(vector<int>& weights, int binCapacity, int& bestBins) {
    bool improvement = false;
    for (int i = 0; i < weights.size(); i++) {
        for (int j = i + 1; j < weights.size(); j++) {
            // Swap items i and j
            swap(weights[i], weights[j]);
            int newBins = firstFit(weights, binCapacity);
            if (newBins < bestBins) {
                bestBins = newBins;
                improvement = true;
            } else {
                // Undo swap if no improvement
                swap(weights[i], weights[j]);
            }
        }
    }
    return improvement;
}

// Swap-Reinsertion neighborhood: Swap two items and reinsert one into another position
bool swapReinsertion(vector<int>& weights, int binCapacity, int& bestBins) {
    bool improvement = false;
    
    for (int i = 0; i < weights.size(); i++) {
        for (int j = i + 1; j < weights.size(); j++) {
            // Swap items i and j
            swap(weights[i], weights[j]);
            
            // Reinsert the swapped item j into a new position
            int temp = weights[j];
            weights.erase(weights.begin() + j);
            int newPos = rand() % weights.size(); // Randomly choose a new position for reinsertion
            weights.insert(weights.begin() + newPos, temp);

            // Check if this swap-reinsertion improves the solution
            int newBins = firstFit(weights, binCapacity);
            if (newBins < bestBins) {
                bestBins = newBins;
                improvement = true;
            } else {
                // Undo the swap-reinsertion if no improvement
                weights.erase(weights.begin() + newPos);
                weights.insert(weights.begin() + j, temp);
                swap(weights[i], weights[j]);
            }
        }
    }

    return improvement;
}


// Improved VND with multiple neighborhoods
int VND(vector<int>& weights, int binCapacity) {
    int bestBins = firstFit(weights, binCapacity);
    bool improvement = true;

    while (improvement) {
        improvement = false;

        // Swap neighborhood
        if (twoOptSwap(weights, binCapacity, bestBins)) {
            improvement = true;
        }

        // Reinsertion neighborhood
        if (reinsertion(weights, binCapacity, bestBins)) {
            improvement = true;
        }

        // Shuffle neighborhood
        if (swapReinsertion(weights, binCapacity, bestBins)) {
            improvement = true;
        }
    }

    return bestBins;
}

// ILS (Iterated Local Search) using the improved VND
int ILS(vector<int> weights, int binCapacity, int maxIterations) {
    int currentBins = VND(weights, binCapacity);
    int bestBins = currentBins;

    for (int iter = 0; iter < maxIterations; iter++) {
        cout << "Iteration " << iter + 1 << ": " << bestBins << " bins" << endl;
        
        // Perturbation: Randomly shuffle some elements
        for (int i = 0; i < weights.size(); i++) {
            if (rand() % 2) {
                int j = rand() % weights.size();
                swap(weights[i], weights[j]);
            }
        }

        // Apply VND on the perturbed solution
        int newBins = VND(weights, binCapacity);

        if (newBins < bestBins) {
            bestBins = newBins;
        }
    }

    return bestBins;
}

int main() {
    srand(time(0));

    ifstream inputFile("./in/binpack1.txt");
    if (!inputFile) {
        cerr << "Error opening file!" << endl;
        return 1;
    }

    ofstream outputFile("./out/binpack1.txt");
    if (!outputFile) {
        cerr << "Error creating output file!" << endl;
        return 1;
    }

    int P; // Number of test problems
    inputFile >> P;

    for (int p = 0; p < P; p++) {
        string problemID;
        int binCapacity, numItems, bestKnownBins;

        // Reading problem identifier and parameters
        inputFile >> problemID >> binCapacity >> numItems >> bestKnownBins;

        vector<int> weights(numItems);
        for (int i = 0; i < numItems; i++) {
            inputFile >> weights[i];
        }

        // Results output
        outputFile << "Problem ID: " << problemID << endl;
        outputFile << "Bin Capacity: " << binCapacity << ", Number of Items: " << numItems << ", Best Known Bins: " << bestKnownBins << endl;

        // Running the algorithms and saving results
        int firstFitResult = firstFit(weights, binCapacity);
        int vndResult = VND(weights, binCapacity);
        int maxIterations = 1000;
        int ilsResult = ILS(weights, binCapacity, maxIterations);

        // Writing results to file
        outputFile << "First-Fit (Greedy): " << firstFitResult << " bins" << endl;
        outputFile << "Improved VND (Heuristic): " << vndResult << " bins" << endl;
        outputFile << "ILS (Metaheuristic): " << ilsResult << " bins" << endl;
        outputFile << "----------------------" << endl;

        // Also printing results to console
        cout << "Problem ID: " << problemID << endl;
        cout << "Bin Capacity: " << binCapacity << ", Number of Items: " << numItems << ", Best Known Bins: " << bestKnownBins << endl;
        cout << "First-Fit (Greedy): " << firstFitResult << " bins" << endl;
        cout << "Improved VND (Heuristic): " << vndResult << " bins" << endl;
        cout << "ILS (Metaheuristic): " << ilsResult << " bins" << endl;
        cout << "----------------------" << endl;
    }

    inputFile.close();
    outputFile.close();

    return 0;
}
