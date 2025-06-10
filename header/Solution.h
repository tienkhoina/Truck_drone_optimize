#ifndef SOLUTION_H
#define SOLUTION_H

#include <iostream>
#include <vector>
#include "Constants.h"
#include <fstream>

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
    void optimize_trip();
    bool operator<(const Solution &other) const;
    bool operator==(const Solution &other) const;
    void invalidate();
    void Tabu_Optimize(int max_iter, int tabu_tenure);
    void print() const;
    void printToFile(const string &filename) const;
};
namespace std
{
    template <>
    struct hash<Solution>
    {
        size_t operator()(const Solution &s) const
        {
            size_t h = 0;
            hash<int> int_hash;
            hash<double> double_hash;

            // Kết hợp băm của các thuộc tính quan trọng
            h ^= int_hash(s.f1) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= double_hash(s.f2) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= int_hash(s.num_drone) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= int_hash(s.num_truck) + 0x9e3779b9 + (h << 6) + (h >> 2);

            return h;
        }
    };
}

#endif 
