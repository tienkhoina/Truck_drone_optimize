#include "Constants.h"
#include "Population.h"
#include <windows.h>

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

    Population pop(num_Custom, 30);
    for(auto m : pop.Mem)
        m.print();
    pop.Genetic("NSGA_II", 0, 30);
    pop.Mem[0].print();
    
    return 0;
}