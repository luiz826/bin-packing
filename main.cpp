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


// Perturbation: Try to merge N random bins and allocate the remaining items randomly
bool mergeAndReallocateNBins(vector<int>& weights, int binCapacity, int& bestBins, int N) {
    vector<int> bins;
    vector<vector<int>> binItems;  // Store items per bin for easy removal
    vector<int> tempWeights = weights;

    // First-Fit to assign items to bins for merging and reallocation step
    for (int weight : tempWeights) {
        bool placed = false;
        for (int i = 0; i < bins.size(); i++) {
            if (bins[i] + weight <= binCapacity) {
                bins[i] += weight;
                binItems[i].push_back(weight);
                placed = true;
                break;
            }
        }
        if (!placed) {
            bins.push_back(weight);
            binItems.push_back({weight});
        }
    }

    random_device rd;
    mt19937 g(rd());

    // Select N random bins
    vector<int> selectedBins;
    while (selectedBins.size() < N && selectedBins.size() < binItems.size()) {
        int binIdx = rand() % binItems.size();
        if (find(selectedBins.begin(), selectedBins.end(), binIdx) == selectedBins.end()) {
            selectedBins.push_back(binIdx);
        }
    }

    // Try to merge selected bins
    for (int i = 0; i < selectedBins.size() - 1; i++) {
        int binIdx1 = selectedBins[i];
        int binIdx2 = selectedBins[i + 1];

        // Merge the two selected bins if possible
        vector<int> mergedBinItems = binItems[binIdx1];
        mergedBinItems.insert(mergedBinItems.end(), binItems[binIdx2].begin(), binItems[binIdx2].end());

        int mergedBinWeight = bins[binIdx1] + bins[binIdx2];

        if (mergedBinWeight <= binCapacity) {
            // Success: Merge bins
            bins[binIdx1] = mergedBinWeight;
            binItems[binIdx1] = mergedBinItems;
            bins.erase(bins.begin() + binIdx2);
            binItems.erase(binItems.begin() + binIdx2);
        } else {
            // Failed merge: Randomly reallocate items from bin2
            vector<int> remainingItems = binItems[binIdx2];
            binItems.erase(binItems.begin() + binIdx2);
            bins.erase(bins.begin() + binIdx2);

            // Randomly allocate remaining items
            for (int item : remainingItems) {
                bool placed = false;

                // Shuffle bins and try to place the item
                shuffle(bins.begin(), bins.end(), g);
                for (int& bin : bins) {
                    if (bin + item <= binCapacity) {
                        bin += item;
                        placed = true;
                        break;
                    }
                }

                // If item cannot be placed, create a new bin
                if (!placed) {
                    bins.push_back(item);
                    binItems.push_back({item});
                }
            }
        }
    }

    // Count the new number of bins
    int newBins = bins.size();

    if (newBins < bestBins) {
        bestBins = newBins;
        return true;
    }

    return false;
}

// Improved VND with multiple neighborhoods
int VND(vector<int>& weights, int binCapacity) {
    int bestBins = firstFit(weights, binCapacity);
    bool improvement = true;

    while (improvement) {
        improvement = false;

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
            } else {
                mergeAndReallocateNBins(weights, binCapacity, bestBins, 6);
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
        int maxIterations = 300;
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
