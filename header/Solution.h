#ifndef SOLUTION_H
#define SOLUTION_H

#include <iostream>
#include <vector>
#include "Constants.h"

using namespace std;

class Solution{
public:
    
    vector<vector<int>> Route;
    vector<int> Role;
    vector<int> Mark; // đánh dấu trip đã được tối ưu hay chưa
    int f1; // hàm mục tiêu 1 ứng với số lượng khách
    double f2; // hàm mục tiêu 2 ứng với chi phí
    int num_drone;
    int num_truck;

    Solution(vector<vector<int>> route,vector<int>role);
    Solution(vector<vector<int>> route, vector<int> role, vector<int> mark);
    Solution(const Solution& other);
    Solution &operator=(const Solution &other);
    bool operator<(const Solution &other) const;
    bool operator==(const Solution &other) const;
    void print();
};
namespace std
{
    template <>
    struct hash<Solution>
    {
        size_t operator()(const Solution &p) const
        {
            return hash<int>()(p.f1) ^ (hash<size_t>()(static_cast<size_t>(p.f2 * 1000)) << 1);
        }
    };
}

#endif 
