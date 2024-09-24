def read_vsbpp_instance(file_name):
    with open(file_name, 'r') as file:
        lines = file.readlines()

    # Extract items
    items_line = lines[1].strip()  # Skip "Items:" label and get the values
    items = list(map(int, items_line.split()))

    # Extract bin capacities
    bin_capacities_line = lines[3].strip()  # Skip "Bin Capacities:" label
    bin_capacities = list(map(int, bin_capacities_line.split()))

    # Extract bin costs
    bin_costs_line = lines[5].strip()  # Skip "Bin Costs:" label
    bin_costs = list(map(int, bin_costs_line.split()))

    return items, bin_capacities, bin_costs

# Example usage to read and get data for the C++ code
items, bin_capacities, bin_costs = read_vsbpp_instance("/home/caio/Downloads/QMC-VSBPP-C/vsbpp/instances/vsbpp_instance_200.txt")

# You can now pass these variables (items, bin_capacities, bin_costs) to the C++ VNS function
