#ifndef MOEA_H
#define MOEA_H

#include <iostream>
#include <vector>

template <typename T1, typename T2>
class MOEA
{
public:
    T1 population;
    MOEA(T1 _population) : population(_population) {}
    void MOEA_Genetic(double mutation_rate, int stagnation,bool Selectparerent2);
};

#endif 
