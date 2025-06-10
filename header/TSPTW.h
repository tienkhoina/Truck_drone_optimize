#ifndef TSPTW_H
#define TSPTW_H

#include <vector>
#include <memory>
#include <string>
#include "ortools/linear_solver/linear_solver.h"

class TSPTW
{
public:
    TSPTW(int n,
          const std::vector<std::vector<double>> &distance_matrix,
          const std::vector<double> &earliest,
          const std::vector<double> &latest,
          const std::vector<double> &service_times,
          double start_time = 0.0);

    virtual std::pair<int, std::vector<int>> solve();

protected:
    void validateData();
    void initializeSolver();
    void createVariables();
    virtual void addConstraints();
    void setObjective();
    virtual std::vector<int> extractSolution() const;
    double computeBigM() const;

    int n_;
    std::vector<std::vector<double>> distance_matrix_;
    std::vector<double> earliest_;
    std::vector<double> latest_;
    std::vector<double> service_times_;
    double start_time_;

    std::unique_ptr<operations_research::MPSolver> solver_;
    std::vector<std::vector<operations_research::MPVariable *>> x_;
    std::vector<operations_research::MPVariable *> t_;
};

#endif // TSPTW_H