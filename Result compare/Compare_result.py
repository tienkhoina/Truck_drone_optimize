import os
import numpy as np
from pymoo.indicators.hv import Hypervolume

# Define the datasets and their respective grid sizes
DATASETS = {
    20: [5, 10, 20],
    50: [10, 20, 30, 40],
    100: [20, 35, 50, 60],
    200: [30, 45, 60, 75]
}

# Algorithm configurations: MOEA/NSGA-II with True/False parent selection
ALGORITHMS = [
    ("MOEA", "true"),
    ("MOEA", "false"),
    ("NSGA", "true"),
    ("NSGA", "false")
]

# Function to parse a file and extract the final Pareto front
def parse_pareto_front(filename):
    pareto_fronts = []
    current_gen = None
    with open(filename, 'r', encoding='utf-8') as f:
        for line in f:
            line = line.strip()
            if line.startswith("Thế hệ thứ:"):
                current_gen = int(line.split(":")[1].strip())
                pareto_fronts.append([])
            else:
                if current_gen is not None and line:
                    parts = line.split()
                    if len(parts) == 2:
                        try:
                            customers, cost = int(parts[0]), float(parts[1])
                            pareto_fronts[-1].append([customers, cost])
                        except (ValueError, IndexError):
                            continue
    if not pareto_fronts or not pareto_fronts[-1]:
        return None
    return np.array(pareto_fronts[-1])

# Function to find the reference point (worst values for both objectives)
def find_reference_point(pareto_fronts):
    if not pareto_fronts:
        raise ValueError("No valid Pareto fronts to compute reference point!")
    all_points = np.vstack(pareto_fronts)
    worst_customers = np.min(all_points[:, 0])  # Smallest number of customers
    worst_cost = np.max(all_points[:, 1])      # Largest cost
    return np.array([-worst_customers * 1.1, worst_cost * 1.1])

# Process all files from the given directory and calculate HV with Gap
def process_directory(directory):
    results = {}
    hv_results = {}

    # Iterate over datasets, grid sizes, and dataset instances (1 to 4)
    for patients, grid_sizes in DATASETS.items():
        for grid in grid_sizes:
            for instance in range(1, 5):  # Assuming 4 instances per grid
                config_key = (patients, grid, instance)
                results[config_key] = {}
                hv_results[config_key] = {}
                all_pareto_fronts = []

                # Collect Pareto fronts for all algorithms for this config
                for algo, parent in ALGORITHMS:
                    filename = f"pareto_front_{patients}_{grid}_{instance}_{algo}_{parent}.txt"
                    file_path = os.path.join(directory, filename)
                    if os.path.exists(file_path):
                        pareto_front = parse_pareto_front(file_path)
                        if pareto_front is not None:
                            all_pareto_fronts.append(pareto_front)
                            results[config_key][(algo, parent)] = pareto_front
                    else:
                        print(f"File not found: {file_path}")

                if not all_pareto_fronts:
                    print(f"No valid Pareto fronts for config {config_key}")
                    continue

                # Find reference point for this config
                reference_point = find_reference_point(all_pareto_fronts)
                hv_calculator = Hypervolume(ref_point=reference_point)

                # Calculate HV for each algorithm
                for algo, parent in ALGORITHMS:
                    if (algo, parent) in results[config_key]:
                        pareto_front = results[config_key][(algo, parent)]
                        points = np.copy(pareto_front)
                        points[:, 0] = -points[:, 0]  # Negate for maximization
                        hv = hv_calculator.do(points)
                        hv_results[config_key][(algo, parent)] = hv
                    else:
                        hv_results[config_key][(algo, parent)] = None

    # Generate the table with GapHV and +/=/- comparison
    print("Table: Performance comparison among algorithms")
    print("| Dataset | Grid size | Instance |", end="")
    for algo, parent in ALGORITHMS:
        print(f" {algo} ({parent}) GapHV (%) | +/=/- |", end="")
    print()
    print("|---------|-----------|----------|", end="")
    for _ in ALGORITHMS:
        print("----------------|--------|", end="")
    print()

    for patients, grid_sizes in DATASETS.items():
        for grid in grid_sizes:
            for instance in range(1, 5):
                config_key = (patients, grid, instance)
                if config_key not in hv_results or all(v is None for v in hv_results[config_key].values()):
                    continue

                # Find the best HV for this config
                hvs = [hv for hv in hv_results[config_key].values() if hv is not None]
                if not hvs:
                    continue
                best_hv = max(hvs)

                row = f"| {patients} | {grid} | {instance} |"
                for algo, parent in ALGORITHMS:
                    hv = hv_results[config_key].get((algo, parent))
                    if hv is not None:
                        gap_hv = ((best_hv - hv) / best_hv) * 100  # GapHV in percentage
                        compare = "=" if abs(gap_hv) < 0.01 else ("+" if gap_hv > 0 else "-")
                        row += f" {gap_hv:.2f} | {compare} |"
                    else:
                        row += " - | - |"
                print(row)

    # Calculate and print averages
    print("\nAverages:")
    avg_gaps = {algo: {parent: [] for parent in ["true", "false"]} for algo in ["MOEA", "NSGA"]}
    for patients, grid_sizes in DATASETS.items():
        for grid in grid_sizes:
            for instance in range(1, 5):
                config_key = (patients, grid, instance)
                if config_key in hv_results:
                    hvs = [hv for hv in hv_results[config_key].values() if hv is not None]
                    if hvs:
                        best_hv = max(hvs)
                        for algo, parent in ALGORITHMS:
                            hv = hv_results[config_key].get((algo, parent))
                            if hv is not None:
                                gap_hv = ((best_hv - hv) / best_hv) * 100
                                avg_gaps[algo][parent].append(gap_hv)

    for patients, grid_sizes in DATASETS.items():
        print(f"\nDataset: {patients}")
        print("| Algorithm | Parent Selection | Average GapHV (%) |")
        print("|-----------|------------------|--------------------|")
        for algo in ["MOEA", "NSGA"]:
            for parent in ["true", "false"]:
                gaps = avg_gaps[algo][parent]
                avg = np.mean(gaps) if gaps else None
                print(f"| {algo} | {parent} | {avg:.2f} |" if avg is not None else f"| {algo} | {parent} | - |")

    # Summary across all datasets
    print("\nSummary:")
    print("| Algorithm | Parent Selection | Average GapHV (%) |")
    print("|-----------|------------------|--------------------|")
    for algo in ["MOEA", "NSGA"]:
        for parent in ["true", "false"]:
            gaps = avg_gaps[algo][parent]
            avg = np.mean(gaps) if gaps else None
            print(f"| {algo} | {parent} | {avg:.2f} |" if avg is not None else f"| {algo} | {parent} | - |")

if __name__ == "__main__":
    directory = r"D:\Project GA new\pareto_result"
    if os.path.isdir(directory):
        process_directory(directory)
    else:
        print("Invalid directory path!")