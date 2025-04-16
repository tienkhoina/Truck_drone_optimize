#ifndef HAMILTON_PATH_H
#define HAMILTON_PATH_H

#include "TSPTW.h"

class HamiltonPath : public TSPTW
{
public:
    HamiltonPath(int n,
                 const std::vector<std::vector<double>> &distance_matrix,
                 const std::vector<double> &earliest,
                 const std::vector<double> &latest,
                 const std::vector<double> &service_times,
                 double start_time = 0.0);

    std::pair<int, std::vector<int>> solve() override;

protected:
    void addConstraints() override;
    std::vector<int> extractSolution() const override;
};

#endif // HAMILTON_PATH_H
