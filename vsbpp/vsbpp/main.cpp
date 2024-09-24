#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <ctime>
#include <fstream>
#include <sstream>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;


struct Bin {
    int capacity;
    int cost;
    int used_capacity;
    vector<int> items;
    
    // Add an item to the bin if there is enough capacity
    bool addItem(int item_size) {
        if (used_capacity + item_size <= capacity) {
            items.push_back(item_size);
            used_capacity += item_size;
            // cout << "Added item " << item_size << " to bin with capacity " << capacity << " used capacity " << used_capacity << endl;
            return true;
        }
        return false;
    }
    
    // Remove an item from the bin
    void removeItem(int item_size) {
        items.erase(remove(items.begin(), items.end(), item_size), items.end());
        used_capacity -= item_size;
        // cout << "Removed item " << item_size << " from bin with capacity " << capacity << " used capacity " << used_capacity << endl;
    }
};

bool fitsInBin(const Bin& bin, int item_size) {
    return bin.used_capacity + item_size <= bin.capacity;
}

int calculateTotalCost(const vector<Bin>& solution) {
    
    int total_cost = 0;
    for (const Bin& bin : solution) {
        if (!bin.items.empty()) {
            total_cost += bin.cost;
        }
    }
    return total_cost;
}

void initializeBins(vector<Bin>& bins, const vector<int>& capacities, const vector<int>& costs) {
    for (size_t i = 0; i < capacities.size(); ++i) {
        bins.push_back({capacities[i], costs[i], 0, {}});
    }
}

vector<Bin> generateInitialSolution(const vector<int>& items, vector<Bin>& bins) {
    vector<Bin> solution = bins;
    for (int item : items) {
        for (Bin& bin : solution) {
            if (bin.addItem(item)) {
                break;
            }
        }
    }
    return solution;
}

// 1. Swap two items between different bins
void swapItems(vector<Bin>& solution) {
    int bin1_idx = rand() % solution.size();
    int bin2_idx = rand() % solution.size();


    if (!solution[bin1_idx].items.empty() && !solution[bin2_idx].items.empty()) {
        int item1_idx = rand() % solution[bin1_idx].items.size();
        int item2_idx = rand() % solution[bin2_idx].items.size();

        if (item1_idx != item2_idx) {

            int item1 = solution[bin1_idx].items[item1_idx];
            int item2 = solution[bin2_idx].items[item2_idx];

            if (item1 != item2) {

                // cout << "Trying to swap items " << item1 << " and " << item2 << " between bins " << bin1_idx << " and " << bin2_idx << endl;

                // Perform the swap if both bins can accommodate the swapped items
                if (solution[bin1_idx].used_capacity - item1 + item2 <= solution[bin1_idx].capacity &&
                    solution[bin2_idx].used_capacity - item2 + item1 <= solution[bin2_idx].capacity) {

                    solution[bin1_idx].removeItem(item1);
                    solution[bin2_idx].removeItem(item2);
                    solution[bin1_idx].addItem(item2);
                    solution[bin2_idx].addItem(item1);

                    // cout << "Total cost after swap: " << calculateTotalCost(solution) << endl;
                }
            }
        }
    }
}


void moveItem(vector<Bin>& solution) {
    int from_bin_idx = rand() % solution.size();
    int to_bin_idx = rand() % solution.size();

    if (!solution[from_bin_idx].items.empty()) {
        int item_idx = rand() % solution[from_bin_idx].items.size();
        int item = solution[from_bin_idx].items[item_idx];

        // cout << "Trying to move item " << item << " from bin " << from_bin_idx << " to bin " << to_bin_idx << endl;

        if (solution[to_bin_idx].used_capacity + item <= solution[to_bin_idx].capacity) {
            solution[from_bin_idx].removeItem(item);
            solution[to_bin_idx].addItem(item);
        }
    }
}

void reassignItems(vector<Bin>& solution) {
    for (Bin& bin : solution) {
        for (int i = 0; i < bin.items.size(); i++) {
            int item = bin.items[i];
            int new_bin_idx = rand() % solution.size();

            // cout << "Trying to reassign item " << item << " from bin " << &bin - &solution[0] << " to bin " << new_bin_idx << endl;
            if (solution[new_bin_idx].used_capacity + item <= solution[new_bin_idx].capacity) {
                bin.removeItem(item);
                solution[new_bin_idx].addItem(item);
            }
        }
    }
}

vector<Bin> VND(vector<Bin> solution) {
    bool improvement = true;
    int neighborhood = 0;
    int num_neighborhoods = 3; 

    while (improvement) {
        improvement = false;
        while (neighborhood < num_neighborhoods) {
            if (neighborhood == 0) {
                swapItems(solution);
            } else if (neighborhood == 1) {
                moveItem(solution);
            } else if (neighborhood == 2) {
                reassignItems(solution);
            }
            if (improvement) {
                neighborhood = 0;  
            } else {
                neighborhood++;  
            }
        }
    }
    return solution;
}

vector<Bin> VNS(vector<Bin> initial_solution, const vector<int>& items, int max_iterations) {
    vector<Bin> best_solution = initial_solution;
    int best_cost = calculateTotalCost(best_solution);

    srand(time(0));  

    for (int iteration = 0; iteration < max_iterations; iteration++) {
        vector<Bin> current_solution = best_solution;

        if (iteration % 3 == 0) {
            swapItems(current_solution);  
        } else if (iteration % 3 == 1) {
            moveItem(current_solution);  
        } else {
            reassignItems(current_solution);  
        }

        // Apply VND for local search
        current_solution = VND(current_solution);

        int current_cost = calculateTotalCost(current_solution);

        if (current_cost < best_cost) {
            best_solution = current_solution;
            best_cost = current_cost;
        }
    }

    return best_solution;
}


void readVSBPPInstance(const string& file_name, vector<int>& items, vector<int>& bin_capacities, vector<int>& bin_costs) {
    ifstream file(file_name);
    string line;

    // Read items
    getline(file, line); // Skip "Items:"
    getline(file, line);
    stringstream ss_items(line);
    int item;
    while (ss_items >> item) {
        items.push_back(item);
    }

    // Read bin capacities
    getline(file, line); // Skip "Bin Capacities:"
    getline(file, line);
    stringstream ss_capacities(line);
    int capacity;
    while (ss_capacities >> capacity) {
        bin_capacities.push_back(capacity);
    }

    // Read bin costs
    getline(file, line); // Skip "Bin Costs:"
    getline(file, line);
    stringstream ss_costs(line);
    int cost;
    while (ss_costs >> cost) {
        bin_costs.push_back(cost);
    }
}


int main() {
    vector<int> items;
    vector<int> bin_capacities;
    vector<int> bin_costs;

    std::string folder_path = "/home/caio/Downloads/QMC-VSBPP-C/vsbpp/instances/"; 
    // Read instance from file
    for (const auto& entry : fs::directory_iterator(folder_path)) {
        if (entry.is_regular_file() && entry.path().extension() == ".txt") {
                readVSBPPInstance(entry.path(), items, bin_capacities, bin_costs);
        }
        cout << "Instance: " << entry.path() << endl;

        vector<Bin> bins;
        initializeBins(bins, bin_capacities, bin_costs);

        vector<Bin> initial_solution = generateInitialSolution(items, bins);

        cout << "Initial bin configuration:\n";
        for (const auto& bin : initial_solution) {
            cout << "Bin (Capacity: " << bin.capacity << ", Cost: " << bin.cost << "): ";
            for (int item : bin.items) {
                cout << item << " ";
            }
            cout << " - Used Capacity: " << bin.used_capacity << endl;
        }
        cout << "Total Cost: " << calculateTotalCost(initial_solution) << endl;

        int total_bins_used_initial = 0;
        for (const auto& bin : initial_solution) {
            if (!bin.items.empty()) {
                total_bins_used_initial++;
            }
        }

        cout << "Total number of bins used: " << total_bins_used_initial << endl;

        int max_iterations = 10000;
        vector<Bin> final_solution = VNS(initial_solution, items, max_iterations);

        cout << "Final bin configuration with minimal cost:\n";
        for (const auto& bin : final_solution) {
            cout << "Bin (Capacity: " << bin.capacity << ", Cost: " << bin.cost << "): ";
            for (int item : bin.items) {
                cout << item << " ";
            }
            cout << " - Used Capacity: " << bin.used_capacity << endl;
        }

        cout << "Total Cost: " << calculateTotalCost(final_solution) << endl;

        int total_bins_used = 0;
        for (const auto& bin : final_solution) {
            if (!bin.items.empty()) {
                total_bins_used++;
            }
        }

        cout << "Total number of bins used: " << total_bins_used << endl;


        ofstream output_file(entry.path().filename());
        if (!output_file) {
            std::cerr << "Error creating output file." << std::endl;
            return 1;
            }

        for (const auto& bin : final_solution) {
            output_file << "Bin (Capacity: " << bin.capacity << ", Cost: " << bin.cost << "): ";
            for (int item : bin.items) {
                output_file << item << " ";
            }
            output_file << " - Used Capacity: " << bin.used_capacity << endl;
        }
        output_file << "Total Cost: " << calculateTotalCost(final_solution) << endl;
        output_file << "Total number of bins used: " << total_bins_used << endl;
        output_file.close();
    }

    return 0;
}