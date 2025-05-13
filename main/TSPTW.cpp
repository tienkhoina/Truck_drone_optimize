#include "TSPTW.h"
#include <algorithm>
#include <stdexcept>
#include <iostream>

TSPTW::TSPTW(int n, const std::vector<std::vector<double>> &distance_matrix,
             const std::vector<double> &earliest, const std::vector<double> &latest,
             const std::vector<double> &service_times, double start_time)
    : n_(n), distance_matrix_(distance_matrix), earliest_(earliest),
      latest_(latest), service_times_(service_times), start_time_(start_time)
{
    // std::cout << "tạo đối tượng" << std::endl;
    validateData();
    // std::cout << "xong" << std::endl;
}

void TSPTW::validateData()
{
    // std::cout << n_ << std::endl;
    // std::cout << distance_matrix_.size() << std::endl;
    if (n_ <= 0)
        throw std::invalid_argument("Number of customers must be positive");

    if (distance_matrix_.size() != n_ + 1)
        throw std::invalid_argument("Distance matrix rows mismatch");

    // std::cout << "không lỗi" << std::endl;

    for (const auto &row : distance_matrix_)
        if (row.size() != n_ + 1)
            throw std::invalid_argument("Distance matrix columns mismatch");

    if (earliest_.size() != n_ + 1 || latest_.size() != n_ + 1 || service_times_.size() != n_ + 1)
        throw std::invalid_argument("Time windows size mismatch");

    for (int i = 0; i <= n_; ++i)
    {
        if (earliest_[i] > latest_[i])
            throw std::invalid_argument("Invalid time window for node " + std::to_string(i));
        if (distance_matrix_[i][i] != 0.0)
            throw std::invalid_argument("Diagonal distance must be zero");
    }
}

double TSPTW::computeBigM() const
{
    return 100;
}

void TSPTW::initializeSolver()
{
    solver_ = std::unique_ptr<operations_research::MPSolver>(
        operations_research::MPSolver::CreateSolver("SCIP"));
}

void TSPTW::createVariables()
{
    x_.resize(n_ + 1, std::vector<operations_research::MPVariable *>(n_ + 1, nullptr));

    for (int i = 0; i <= n_; ++i)
    {
        for (int j = 0; j <= n_; ++j)
        {
            if (i != j)
            {
                x_[i][j] = solver_->MakeBoolVar("x_" + std::to_string(i) + "_" + std::to_string(j));
                if (x_[i][j] == nullptr)
                {
                    std::cerr << "Không thể tạo biến x_" << i << "_" << j << "\n";
                }
            }
        }
    }

    t_.resize(n_ + 1);
    for (int i = 0; i <= n_; ++i)
    {
        t_[i] = solver_->MakeNumVar(earliest_[i], latest_[i], "t_" + std::to_string(i));
    }
    t_[0]->SetBounds(start_time_, start_time_);
}

void TSPTW::addConstraints()
{
    // Tìm khách hàng có thời gian earliest nhỏ nhất (bỏ qua depot là 0)
    // Ràng buộc khách hàng đầu tiên có thời gian earliest nhỏ nhất
    int min_earliest_customer = 1;
    for (int i = 2; i <= n_; ++i)
    {
        if (earliest_[i] < earliest_[min_earliest_customer])
        {
            min_earliest_customer = i;
        }
    }

    // Đảm bảo đi từ depot đến khách hàng này đầu tiên
    if (x_[0][min_earliest_customer] != nullptr)
    {
        x_[0][min_earliest_customer]->SetBounds(1.0, 1.0);
    }

    // Flow constraints
    for (int i = 1; i <= n_; ++i)
    {
        operations_research::MPConstraint *incoming_constraint = solver_->MakeRowConstraint(1.0, 1.0, "in_" + std::to_string(i));
        operations_research::MPConstraint *outgoing_constraint = solver_->MakeRowConstraint(1.0, 1.0, "out_" + std::to_string(i));

        for (int j = 0; j <= n_; ++j)
        {

            if (i != j)
            {
                if (x_[j][i] == nullptr || x_[i][j] == nullptr)
                {
                    std::cerr << "Không thể tạo biến x_" << j << "_" << i << "\n";
                    continue;
                }

                incoming_constraint->SetCoefficient(x_[j][i], 1.0);
                outgoing_constraint->SetCoefficient(x_[i][j], 1.0);
            }
        }
    }

    // Depot constraints
    operations_research::MPConstraint *depart_constraint = solver_->MakeRowConstraint(1.0, 1.0, "depart");
    operations_research::MPConstraint *ret_constraint = solver_->MakeRowConstraint(1.0, 1.0, "return");

    for (int j = 1; j <= n_; ++j)
    {
        depart_constraint->SetCoefficient(x_[0][j], 1.0);
        ret_constraint->SetCoefficient(x_[j][0], 1.0);
    }

    // Time constraints
    const double M = computeBigM();
    for (int i = 0; i <= n_; ++i)
    {
        for (int j = 1; j <= n_; ++j)
        {
            if (i != j && x_[i][j] != nullptr)
            {
                auto *ct = solver_->MakeRowConstraint(
                    -solver_->infinity(),
                    M - distance_matrix_[i][j] - service_times_[i],
                    "time_" + std::to_string(i) + "_" + std::to_string(j));

                ct->SetCoefficient(t_[i], 1.0);
                ct->SetCoefficient(t_[j], -1.0);
                ct->SetCoefficient(x_[i][j], M);
            }
        }
    }
}

void TSPTW::setObjective()
{
    operations_research::MPObjective *objective = solver_->MutableObjective();
    objective->SetMinimization();

    for (int i = 0; i <= n_; ++i)
    {
        for (int j = 0; j <= n_; ++j)
        {
            if (i != j && x_[i][j] != nullptr)
            {
                objective->SetCoefficient(x_[i][j], distance_matrix_[i][j]);
            }
        }
    }
}

std::vector<int> TSPTW::extractSolution() const
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
            if (!visited[j] && x_[current][j]->solution_value() > 0.99)
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

    // Return to depot
    if (x_[current][0]->solution_value() > 0.99)
    {
        solution.push_back(0);
    }

    if (solution.size() != n_ + 2)
    {
        throw std::runtime_error("Invalid solution: incomplete route");
    }

    return solution;
}

std::pair<int, std::vector<int>> TSPTW::solve()
{

    initializeSolver();

    createVariables();

    addConstraints();

    setObjective();

    const auto status = solver_->Solve();
    operations_research::MPObjective *objective = solver_->MutableObjective();

    switch (status)
    {
    case operations_research::MPSolver::OPTIMAL:
        // std::cout << "Optimal solution found! Cost: "
        //  << objective->Value() << "\n";
        return {0, extractSolution()};

    case operations_research::MPSolver::FEASIBLE:
        // std::cout << "Feasible solution found (gap: "
        //<< objective->BestBound() << ")\n";
        return {1, extractSolution()};

    default:
        // std::cerr << "Solver status: " << status << "\n";
        // std::cerr << "Best bound: " << objective->BestBound() << "\n";
        return {-1, {}};
    }
}
