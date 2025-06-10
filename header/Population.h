#ifndef POPULATION_H
#define POPULATION_H

#include "Constants.h"
#include <iostream>
#include"Solution.h"
#include <vector>
using namespace std;

class Population
{
public:
    int n;// số khách hàng tối đa
    int size; // kích thước quần thể
    vector<Solution> Mem;
    bool opt;

    Population(int _n, int _size, bool _opt = false);
    void basic_sort();
    void sort_by_domination_crowdingdistance();
    Solution TNselection(int k = 3);
    Solution crossover(Solution parent1, Solution parent2);
    Solution mutate(Solution parents, double mutation_rate);
    
    Solution SelectByParent1(Solution parent1);
    void removeSame();
    void create_next_member();
    void Genetic(string Method, double mutation_rate, int stagnation, bool Selectparent2);
};



#endif