#include "Hamilton_path.h"

HamiltonPath::HamiltonPath(int n,
                           const std::vector<std::vector<double>> &distance_matrix,
                           const std::vector<double> &earliest,
                           const std::vector<double> &latest,
                           const std::vector<double> &service_times,
                           double start_time)
    : TSPTW(n, distance_matrix, earliest, latest, service_times, start_time)
{
}

// Ghi đè constraints: loại bỏ điều kiện quay về depot
void HamiltonPath::addConstraints()
{
    // Flow constraints
    for (int i = 1; i <= n_; ++i)
    {
        operations_research::MPConstraint *incoming = solver_->MakeRowConstraint(1.0, 1.0, "in_" + std::to_string(i));
        operations_research::MPConstraint *outgoing = solver_->MakeRowConstraint(1.0, 1.0, "out_" + std::to_string(i));

        for (int j = 0; j <= n_; ++j)
        {
            if (i != j)
            {
                incoming->SetCoefficient(x_[j][i], 1.0);
                outgoing->SetCoefficient(x_[i][j], 1.0);
            }
        }
    }

    // Depot constraints
    operations_research::MPConstraint *depart = solver_->MakeRowConstraint(1.0, 1.0, "depart");
    for (int j = 1; j <= n_; ++j)
    {
        depart->SetCoefficient(x_[0][j], 1.0);
    }

    // Time constraints
    const double M = computeBigM();
    for (int i = 0; i <= n_; ++i)
    {
        for (int j = 1; j <= n_; ++j)
        {
            if (i != j && x_[i][j] != nullptr)
            {
                auto *ct = solver_->MakeRowConstraint(-solver_->infinity(), M - distance_matrix_[i][j] - service_times_[i], "time_" + std::to_string(i) + "_" + std::to_string(j));

                ct->SetCoefficient(t_[i], 1.0);
                ct->SetCoefficient(t_[j], -1.0);
                ct->SetCoefficient(x_[i][j], M);
            }
        }
    }
}

// Ghi đè extractSolution để không thêm depot ở cuối
std::vector<int> HamiltonPath::extractSolution() const
{
    std::vector<int> solution;
    std::vector<bool> visited(n_ + 1, false);
    int current = 0;
    solution.push_back(current);
    visited[current] = true;

    while (solution.size() <= n_)
    {
        bool found = false;
        for (int j = 0; j <= n_; ++j)
        {
            if (!visited[j] && x_[current][j] != nullptr && x_[current][j]->solution_value() > 0.99)
            {
                current = j;
                solution.push_back(current);
                visited[current] = true;
                found = true;
                break;
            }
        }
        if (!found)
            break;
    }

    if (solution.size() != n_ + 1) // depot + n điểm
        throw std::runtime_error("Invalid Hamilton path: incomplete path");

    return solution;
}

// Ghi đè solve để sử dụng các hàm đã override
std::pair<int, std::vector<int>> HamiltonPath::solve()
{
    initializeSolver();
    createVariables();
    addConstraints(); // dùng hàm override
    setObjective();

    const auto status = solver_->Solve();
    operations_research::MPObjective *objective = solver_->MutableObjective();

    switch (status)
    {
    case operations_research::MPSolver::OPTIMAL:
        return {0, extractSolution()};
    case operations_research::MPSolver::FEASIBLE:
        return {1, extractSolution()};
    default:
        return {-1, {}};
    }
}
