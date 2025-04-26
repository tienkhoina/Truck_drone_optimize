#ifndef NSGA_H
#define NSGA_H

#include <iostream>
#include <vector>

template <typename T1, typename T2>
class NSGA_II
{
public:
    T1 &population;

    NSGA_II(T1 &_population) : population(_population) {}

    void NSGA_II_Genetic(double mutation_rate, int stagnation,bool Selectparerent2);
};

#endif // NSGA_H
