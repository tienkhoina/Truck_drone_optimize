#ifndef TSPTW_H
#define TSPTW_H

#include <vector>
#include <iostream>
#include <memory>
#include <stdexcept>
#include "ortools/linear_solver/linear_solver.h"

using namespace operations_research;
using namespace std;

class TSPTW
{
public:
    int n;                    // Số lượng khách hàng
    vector<vector<double>> C; // Ma trận chi phí di chuyển
    vector<double> e, l, d;   // e(i): thời gian mở cửa, l(i): thời gian đóng cửa, d(i): thời gian phục vụ
    unique_ptr<MPSolver> solver;
    vector<vector<MPVariable *>> x; // Biến quyết định x[i][j]
    vector<MPVariable *> t;         // Biến thời gian đến t[i]

    // Constructor
    TSPTW(int _n, const vector<vector<double>> &_C,
          const vector<double> &_e, const vector<double> &_l, const vector<double> &_d);

    // Các hàm chính
    void addConstraints();
    void solve();
    vector<int> getSolution();

private:
    vector<vector<int>> findSubtours();
    void addSubtourConstraints(const vector<vector<int>> &subtours);
};

#endif