#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <ctime>

void generate_instance(const std::string& filename, int num_items, int num_bins) {
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "Error creating file: " << filename << std::endl;
        return;
    }

    // Write number of items and bins
    file << "Items: " << std::endl;
    // Generate item sizes (random between 1 and 20)
    for (int i = 0; i < num_items; ++i) {
        file << (rand() % 20 + 1) << " "; // Random size from 1 to 20
    }

    file << std::endl << "Bin Capacities: " << std::endl;
    // Generate bin capacities (random between 10 and 50)
    for (int i = 0; i < num_bins; ++i) {
        file << (rand() % 41 + 20) << " "; // Random capacity from 10 to 50
    }

    file << std::endl << "Bin Costs: " << std::endl;
    // Generate bin costs (random between 1 and 15)
    for (int i = 0; i < num_bins; ++i) {
        file << (rand() % 20 + 5) << " "; // Random cost from 1 to 15
    }
    file.close();
}

int main() {
    srand(static_cast<unsigned>(time(0))); // Seed for random number generation

    generate_instance("instances/instance_50.txt", 50, 25);
    generate_instance("instances/instance_100.txt", 100, 50);
    generate_instance("instances/instance_200.txt", 200, 100);
    generate_instance("instances/instance_500.txt", 500, 250);

    return 0;
}
