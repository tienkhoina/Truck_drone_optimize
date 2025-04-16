#include "Constants.h"
#include "Population.h"
#include <windows.h>
#include "function.h"

int main()
{
    SetConsoleOutputCP(65001);
    freopen("D:\\Project GA new\\data\\Truck_drone.txt", "r", stdin);

    cin >> num_Custom;
    Cus.resize(num_Custom + 1);
    Cus[0] = Customer(0, INT_MAX, 0, 0);
    for (int i = 1; i <= num_Custom; i++)
    {
        cin >> Cus[i].start >> Cus[i].end >> Cus[i].weight >> Cus[i].role;
    }

    Ex.resize(num_Custom + 1, vector<double>(num_Custom + 1));

    cin >> Max_Weight_Truck >> Max_Weight_Drone >> Max_Energy >> t_Truck >> t_drone >> k_Truck >> k_drone >> en_drone >> cost_truck >> cost_drone >> max_truck >> max_drone;

    for (int i = 0; i <= num_Custom; i++)
    {
        for (int j = 0; j <= num_Custom; j++)
            cin >> Ex[i][j];
    }

    for (int i = 1; i <= num_Custom; i++)
    {
        cout << Cus[i].start << " " << Cus[i].end << " " << Cus[i].weight << " " << Cus[i].role << endl;
    }
    
    

    cout << Max_Weight_Truck << " " << Max_Weight_Drone << " " << Max_Energy << endl;
    cout << t_Truck <<" "<< t_drone <<" "<<  k_Truck <<" "<< k_drone <<" "<< en_drone <<" "<< cost_truck <<" " << cost_drone << " " << max_truck <<" "<< max_drone << endl;
    for (int i = 0; i <= num_Custom; i++)
    {
        for (int j = 0; j <= num_Custom; j++)
            cout << Ex[i][j] << " ";
        cout << endl;
    }

    

    cout << int(checkValidVector({0, 5,4,13,16,0}, 0, 0)) << endl;
    cout << int(checkValidVector({0, 4,5, 16, 13, 0}, 0, 0)) << endl;

    auto solut = solverTSPTWmapping({0, 5,4,13,16,0}, Ex, t_Truck, 0);

    for (int x : solut.second)
    {
        cout << x << " ";
    }
    cout << endl;
    Population pop(num_Custom, 50);

    // cout << "cá thể 1: " << endl;
    // pop.Mem[0].print();

    pop.Genetic("NSGA_II", 0.4, 50);

    cout << "Ok" << endl;

    for (int i = 0; i < 10;i++)
        pop.Mem[i].print();

    printParetoFront(pareto_front_in_generation, "D:\\Project GA new\\data\\Pareto1.txt");
    return 0;
}