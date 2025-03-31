#include "NSGA.h"
#include <iostream>
#include <vector>
#include "Population.h"
#include "Solution.h"
using namespace std;

template <typename T1, typename T2>
void NSGA_II<T1, T2>::NSGA_II_Genetic(double mutation_rate, int stagnation)
{
    cout << "Start genetic algorithm..." << endl;
    for (int i = 0; i < stagnation; i++)
    {
        cout << "Generation " << i << endl;
        vector<T2> Child;

        while (Child.size() < population.size) // Giả sử size() là một phương thức hợp lệ
        {
            T2 parent1 = population.TNselection();
            T2 parent2 = population.TNselection();


            T2 child1 = population.crossover(parent1, parent2);
            T2 child2 = population.crossover(parent2, parent1);


            child1 = population.mutate(child1, mutation_rate);
            child2 = population.mutate(child2, mutation_rate);

            Child.push_back(child1);
            Child.push_back(child2);
        }

        for (auto &x : Child)
        {
            population.Mem.push_back(x);
        }

        population.sort_by_domination_crowdingdistance();
        population.create_next_member();
    }
}

template class NSGA_II<Population, Solution>;
