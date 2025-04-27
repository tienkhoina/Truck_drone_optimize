#include "Constants.h"
#include "Population.h"
#include <windows.h>
#include "function.h"
#include <string>

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


    // cout << endl;
    // Population pop(num_Custom, 50);
    
    // cout << Cus[45].start << " " << Cus[45].end << endl;
    // cout << 0 << " " << t_Truck * Ex[0][12] <<" "<< t_Truck * Ex[0][42] << " " << t_Truck * Ex[0][45] << endl;
    // cout << t_Truck * Ex[12][0] << " "<< 0 << " " << t_Truck * Ex[12][42] << " " << t_Truck * Ex[12][45] << endl;
    // cout << t_Truck * Ex[42][0] << " " << t_Truck * Ex[42][12] << " " << 0 << " " << t_Truck * Ex[42][45] << endl;
    // cout << t_Truck * Ex[45][0] << " " << t_Truck * Ex[45][12] << " " << t_Truck * Ex[45][42] << " " << 0 << endl;
    // // // cout << "cá thể 1: " << endl;
    // // pop.Mem[0].print();
    // cout << int(checkValidVector({0 ,26 ,22 ,1 ,49 ,34 ,37 ,12 ,31 ,32 ,40, 0}, 0, 0));
    // cout << int(checkValidVector({0 ,49 ,12 ,1 ,22 ,32, 31 ,26 ,37 ,34 ,40 ,0}, 0, 0));

    // auto solut = solverTSPTWmappingLarge({0, 17, 15, 2, 36, 5, 25, 6, 45, 24, 34, 1, 11, 31, 13, 40, 8, 0}, Ex, t_Truck, 0, 10).second;
    // cout << "TSPTW: " << endl;
    // for (int i = 0; i < solut.size(); i++){
    //     cout << solut[i] << " ";
    // }
    // cout << int(checkValidVector(solut, 0, 0)) << endl;
    // pop.Genetic("MOEA", 0.4, 30,false);
    // vector<vector<int>> Route_father = {
    //     {0, 2, 1, 11, 6, 5, 13, 40, 3, 8, 31, 0},
    //     {0, 35, 38, 23, 0},
    //     {0, 30, 0},
    //     {0, 28, 39, 0},
    //     {0, 37, 0}};

    // vector<int> Role_father = {
    //     0, // Truck
    //     1, // Drone 1
    //     1, // Drone 1
    //     2, // Drone 2
    //     2  // Drone 2
    // };

    // Solution father(Route_father, Role_father);
    // cout << "Cha: " << endl;
    // father.print();
    // vector<vector<int>> Route_mother = {
    //     {0, 9, 0},
    //     {0, 40, 0},
    //     {0, 28, 37, 0},
    //     {0, 30, 0},
    //     {0, 39, 0},
    //     {0, 35, 38, 23, 0},
    //     {0, 4, 0}};

    // vector<int> Role_mother = {
    //     0, // Truck
    //     1, // Drone 1
    //     2, // Drone 2
    //     2, // Drone 2
    //     2, // Drone 2
    //     3, // Drone 3
    //     3  // Drone 3
    // };

    // Solution mother(Route_mother, Role_mother);
    // cout << "Mẹ: " << endl;
    // mother.print();

    // auto child = pop.crossover(father, mother);
    // cout << "Con: " << endl;
    // child.print();
    // cout << "Ok" << endl;
    // auto child = pop.crossover(pop.Mem[0], pop.Mem[10]);
    // cout << "Cha: " << endl;
    // pop.Mem[0].print();
    // cout << "Mẹ: " << endl;
    // pop.Mem[10].print();
    // cout << "Con: " << endl;
    // child.print();


    
    // cout << "Sau tối ưu: " << endl;
    // pop.Mem[0].print();

    // printParetoFront(pareto_front_in_generation, "D:\\Project GA new\\data\\Pareto1_Moea_without_select_parent2_20.txt");
    // pareto_front_in_generation.clear();

    // Population pop2(num_Custom, 50);
    // pop2.Genetic("NSGA_II", 0.4, 30,true);
    // printParetoFront(pareto_front_in_generation, "D:\\Project GA new\\data\\Pareto1_with_ select _parent_20.txt");

    // pareto_front_in_generation.clear();
    // Population pop3(num_Custom, 50);
    // pop3.Genetic("NSGA_II", 0.4, 30, false);
    // printParetoFront(pareto_front_in_generation, "D:\\Project GA new\\data\\Pareto1_20.txt");

    // pareto_front_in_generation.clear();
    // Population pop4(num_Custom, 50);
    // pop4.Genetic("MOEA", 0.4, 30, true);
    // printParetoFront(pareto_front_in_generation, "D:\\Project GA new\\data\\Pareto1_Moea_with_select_parent2_0.txt");

    // while(max_truck >=0){
    //     pareto_front_in_generation.clear();
        Population pop(num_Custom, 20);
        pop.Genetic("NSGA_II", 0.4, 30, true);
        printParetoFront(pareto_front_in_generation, "D:\\Project GA new\\data\\Pareto1_Nsga_with_select_parent2_0.txt");
    //     max_truck--;
    //     max_drone += 3;
    // }
    return 0;
}