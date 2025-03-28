#include "TSPTW.h"
#include <algorithm>
#include <limits>

TSPTW::TSPTW(int _n, const vector<vector<double>> &_C,
             const vector<double> &_e, const vector<double> &_l, const vector<double> &_d)
    : n(_n), C(_C), e(_e), l(_l), d(_d),
      solver(MPSolver::CreateSolver("SCIP"))
{ // Khởi tạo trực tiếp trong danh sách khởi tạo
    if (!solver)
    {
        throw runtime_error("SCIP solver unavailable");
    }
}

void TSPTW::addConstraints()
{
    // Thiết lập hàm mục tiêu
    MPObjective *const objective = solver->MutableObjective();
    objective->SetMinimization();

    // Khởi tạo biến
    x.resize(n + 1, vector<MPVariable *>(n + 1, nullptr));
    t.resize(n + 1, nullptr);

    // Tạo biến quyết định x[i][j] (binary)
    for (int i = 0; i <= n; ++i)
    {
        for (int j = 0; j <= n; ++j)
        {
            if (i != j)
            {
                x[i][j] = solver->MakeBoolVar("x_" + to_string(i) + "_" + to_string(j));
                objective->SetCoefficient(x[i][j], C[i][j]);
            }
        }

        // Tạo biến thời gian đến t[i]
        double lb = max(e[i], 0.0);
        double ub = l[i];
        t[i] = solver->MakeNumVar(lb, ub, "t_" + to_string(i));
    }

    // Ràng buộc vào-ra mỗi điểm (trừ kho)
    for (int i = 1; i <= n; ++i)
    {
        LinearExpr incoming, outgoing;
        for (int j = 0; j <= n; ++j)
        {
            if (i != j)
            {
                if (x[j][i])
                    incoming += x[j][i];
                if (x[i][j])
                    outgoing += x[i][j];
            }
        }
        solver->MakeRowConstraint(incoming == 1, "in_" + to_string(i));
        solver->MakeRowConstraint(outgoing == 1, "out_" + to_string(i));
    }

    // Ràng buộc kho (điểm 0)
    LinearExpr departures, returns;
    for (int j = 1; j <= n; ++j)
    {
        if (x[0][j])
            departures += x[0][j];
        if (x[j][0])
            returns += x[j][0];
    }
    solver->MakeRowConstraint(departures == 1, "depart");
    solver->MakeRowConstraint(returns == 1, "return");

    // Ràng buộc thời gian (MTZ)
    const double M = 1e6;
    for (int i = 0; i <= n; ++i)
    {
        for (int j = 1; j <= n; ++j)
        {
            if (i != j && x[i][j])
            {
                MPConstraint *const ct = solver->MakeRowConstraint(
                    -solver->infinity(), M - d[i] - C[i][j]);
                ct->SetCoefficient(t[i], 1);
                ct->SetCoefficient(t[j], -1);
                ct->SetCoefficient(x[i][j], M);
            }
        }
    }
}

vector<vector<int>> TSPTW::findSubtours()
{
    vector<int> visited(n + 1, false);
    vector<vector<int>> subtours;

    for (int i = 1; i <= n; ++i)
    {
        if (!visited[i])
        {
            vector<int> subtour;
            int current = i;
            while (!visited[current])
            {
                visited[current] = true;
                subtour.push_back(current);
                for (int j = 1; j <= n; ++j)
                {
                    if (x[current][j] && x[current][j]->solution_value() > 0.5)
                    {
                        current = j;
                        break;
                    }
                }
            }
            if (subtour.size() > 1 && subtour.size() < n)
            {
                subtours.push_back(subtour);
            }
        }
    }
    return subtours;
}

void TSPTW::addSubtourConstraints(const vector<vector<int>> &subtours)
{
    for (const auto &subtour : subtours)
    {
        LinearExpr expr;
        for (int i : subtour)
        {
            for (int j : subtour)
            {
                if (i != j && x[i][j])
                {
                    expr += x[i][j];
                }
            }
        }
        solver->MakeRowConstraint(expr <= subtour.size() - 1);
    }
}

void TSPTW::solve()
{
    addConstraints();
    const int max_iterations = 100;
    int iteration = 0;

    while (iteration++ < max_iterations)
    {
        const MPSolver::ResultStatus status = solver->Solve();

        if (status != MPSolver::OPTIMAL && status != MPSolver::FEASIBLE)
        {
            cerr << "No solution found in iteration " << iteration << endl;
            break;
        }

        auto subtours = findSubtours();
        if (subtours.empty())
        {
            cout << "Found optimal solution after " << iteration << " iterations" << endl;
            break;
        }

        addSubtourConstraints(subtours);
    }
}

vector<int> TSPTW::getSolution()
{
    vector<int> path;
    if (x.empty())
        return path;

    int current = 0;
    path.push_back(current);

    while (true)
    {
        bool found = false;
        for (int j = 0; j <= n; ++j)
        {
            if (j != current && x[current][j] && x[current][j]->solution_value() > 0.5)
            {
                current = j;
                path.push_back(current);
                found = true;
                break;
            }
        }
        if (!found || current == 0)
            break;
    }

    return path;
}