import os
import numpy as np
import pandas as pd
from pymoo.indicators.hv import HV

# === CẤU HÌNH ===
algorithms = ["NSGA_true", "NSGA_false", "MOEA_true", "MOEA_false"]

dataset_config = {
    20: [5, 10, 20],
    50: [10, 20, 30, 40],
    100: [10, 20, 30, 40],
    200: [10, 20, 30, 40]
}

base_path = r"D:\Project GA new\pareto_result"  # ⚠️ Đổi thành đường dẫn thư mục chứa các file txt

# === HÀM ĐỌC FILE ===
def read_last_pareto(filepath):
    pareto = []
    with open(filepath, 'r', encoding='utf-8') as f:
        lines = f.readlines()
    current = []
    for line in lines:
        if line.startswith("Thế hệ"):
            current = []
        else:
            parts = line.strip().split()
            if len(parts) == 2:
                current.append([int(parts[0]), float(parts[1])])
    return np.array(current)

# === TÍNH HV CHO MỖI CASE ===
def compute_hv_for_case(n_customers, grid, data_id, algorithms, base_path):
    paretos = {}
    for algo in algorithms:
        filename = f"pareto_front_{n_customers}_{grid}_{data_id}_{algo}.txt"
        filepath = os.path.join(base_path, filename)
        if not os.path.exists(filepath):
            print(f"[!] File not found: {filename}")
            return None
        front = read_last_pareto(filepath)
        front[:, 0] = -front[:, 0]  # Đảo dấu: max -> min
        paretos[algo] = front

    # Gộp tất cả để tính điểm tồi nhất
    all_points = np.vstack(list(paretos.values()))
    mins = np.min(all_points, axis=0)
    maxs = np.max(all_points, axis=0)
    ranges = maxs - mins
    ranges[ranges == 0] = 1  # Tránh chia cho 0

    # Chuẩn hóa và tính HV
    ref_point = np.array([1, 1])
    hvs = {}
    for algo, front in paretos.items():
        norm_front = (front - mins) / ranges
        hv = HV(ref_point=ref_point).do(norm_front)
        hvs[algo] = hv
    return hvs

print(compute_hv_for_case(20, 5, 2, algorithms, base_path))

# === THU THẬP TOÀN BỘ KẾT QUẢ ===
def collect_results(base_path):
    results = []
    for n_customers, grids in dataset_config.items():
        for grid in grids:
            for data_id in range(1,5):  # 4 bộ dữ liệu mỗi case
                hvs = compute_hv_for_case(n_customers, grid, data_id, algorithms, base_path)
                if hvs is not None:
                    results.append({
                        "customers": n_customers,
                        "grid": grid,
                        "data_id": data_id,
                        **hvs
                    })
    return pd.DataFrame(results)

# === TẠO BẢNG TỔNG HỢP GIỐNG TABLE 1 ===
def build_summary_table(df, base_algo="NSGA_true"):
    summary = []
    group_cols = ["customers", "grid"]
    for (cust, grid), group in df.groupby(group_cols):
        base_hv = group[base_algo].values
        row = {
            "Customer": cust,
            "Grid size": grid
        }
        for algo in algorithms:
            algo_hv = group[algo].values
            gap = 100 * (algo_hv - base_hv) / base_hv
            win = np.sum(algo_hv > base_hv)
            lose = np.sum(algo_hv < base_hv)
            row[f"{algo}_HV"] = np.round(np.mean(algo_hv), 2)
            row[f"{algo}_GapHV"] = np.round(np.mean(gap), 2)
            row[f"{algo}_+/-"] = f"{win}/{lose}"
        summary.append(row)
    return pd.DataFrame(summary)

# === CHẠY TOÀN BỘ ===
raw_df = collect_results(base_path)
summary_table = build_summary_table(raw_df)

# === XEM KẾT QUẢ ===
pd.set_option("display.max_columns", None)
print(summary_table)  # In 10 dòng đầu tiên
# # Hoặc lưu ra Excel
# # summary_table.to_excel("hv_summary_table.xlsx", index=False)
