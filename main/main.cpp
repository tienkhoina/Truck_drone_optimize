#include "Constants.h"
#include "Population.h"
#include <windows.h>
#include "function.h"
#include <string>

int main()
{
    SetConsoleOutputCP(65001);
    vector<int> num_customers = {20, 50, 100, 200};
    vector<vector<double>> grid = {{5, 10, 20}, {10,20,30,40}, { 10,20,30,40}, {10,20,30,40}};

    for (int in = 0; in < num_customers.size(); ++in)
    {
        for (double x : grid[in])
        {
            for (int jn = 1; jn <= 4; jn++)
            {
                string data_file_name;
                data_file_name= "D:\\Project GA new\\data\\Data_" + to_string(num_customers[in]) + "_" + to_string(int(x)) + "_" + to_string(jn) + ".txt";
                freopen(data_file_name.c_str(), "r", stdin);
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
                cout << t_Truck << " " << t_drone << " " << k_Truck << " " << k_drone << " " << en_drone << " " << cost_truck << " " << cost_drone << " " << max_truck << " " << max_drone << endl;
                for (int i = 0; i <= num_Custom; i++)
                {
                    for (int j = 0; j <= num_Custom; j++)
                        cout << Ex[i][j] << " ";
                    cout << endl;
                }
                string file_pareto_result_1, file_pareto_result_2, file_pareto_result_3, file_pareto_result_4,
                    file_solution_result_1, file_solution_result_2, file_solution_result_3, file_solution_result_4;
                file_pareto_result_1 = "D:\\Project GA new\\pareto_result\\pareto_front_" + to_string(num_customers[in]) + "_" + to_string(int(x)) + "_" + to_string(jn) + "_NSGA_true" + ".txt";
                file_pareto_result_2 = "D:\\Project GA new\\pareto_result\\pareto_front_" + to_string(num_customers[in]) + "_" + to_string(int(x)) + "_" + to_string(jn) + "_NSGA_false" + ".txt";
                file_pareto_result_3 = "D:\\Project GA new\\pareto_result\\pareto_front_" + to_string(num_customers[in]) + "_" + to_string(int(x)) + "_" + to_string(jn) + "_MOEA_true" + ".txt";
                file_pareto_result_4 = "D:\\Project GA new\\pareto_result\\pareto_front_" + to_string(num_customers[in]) + "_" + to_string(int(x)) + "_" + to_string(jn) + "_MOEA_false" + ".txt";

                file_solution_result_1 = "D:\\Project GA new\\solution_result\\solution_" + to_string(num_customers[in]) + "_" + to_string(int(x)) + "_" + to_string(jn) + "_NSGA_true" + ".txt";
                file_solution_result_2 = "D:\\Project GA new\\solution_result\\solution_" + to_string(num_customers[in]) + "_" + to_string(int(x)) + "_" + to_string(jn) + "_NSGA_false" + ".txt";
                file_solution_result_3 = "D:\\Project GA new\\solution_result\\solution_" + to_string(num_customers[in]) + "_" + to_string(int(x)) + "_" + to_string(jn) + "_MOEA_true" + ".txt";
                file_solution_result_4 = "D:\\Project GA new\\solution_result\\solution_" + to_string(num_customers[in]) + "_" + to_string(int(x)) + "_" + to_string(jn) + "_MOEA_false" + ".txt";


                int num_gen = (num_Custom <= 20) ? 30 : 50;

                int pop_size = (num_Custom <= 20) ? 30 : 50;

                auto pop1 = Population(num_Custom, pop_size,true);



                auto pop2 = Population(num_Custom, pop_size);
                auto pop3 = Population(num_Custom, pop_size);
                auto pop4 = Population(num_Custom, pop_size);

                pop1.Genetic("NSGA_II", 0.4, num_gen, true);
              
                int size_pareto_front = pareto_front_in_generation.back().size();

                for(int index = 0; index < size_pareto_front; ++index)
                {
                    pop1.Mem[index].printToFile(file_solution_result_1);
                }

                printParetoFront(pareto_front_in_generation, file_pareto_result_1);
                pareto_front_in_generation.clear();

                pop2.Genetic("NSGA_II", 0.4, num_gen, false);
           
                size_pareto_front = pareto_front_in_generation.back().size();
                for(int index = 0; index < size_pareto_front; ++index)
                {
                    pop2.Mem[index].printToFile(file_solution_result_2);
                }
                printParetoFront(pareto_front_in_generation, file_pareto_result_2);
                pareto_front_in_generation.clear();

                pop3.Genetic("MOEA", 0.4, num_gen, true);
           
                size_pareto_front = pareto_front_in_generation.back().size();
                for(int index = 0; index < size_pareto_front; ++index)
                {
                    pop3.Mem[index].printToFile(file_solution_result_3);
                }
                printParetoFront(pareto_front_in_generation, file_pareto_result_3);
                pareto_front_in_generation.clear();

                pop4.Genetic("MOEA", 0.4, num_gen, false);
                
                size_pareto_front = pareto_front_in_generation.back().size();
                for(int index = 0; index < size_pareto_front; ++index)
                {
                    pop4.Mem[index].printToFile(file_solution_result_4);
                }
                printParetoFront(pareto_front_in_generation, file_pareto_result_4);
                pareto_front_in_generation.clear();
    //             cout << "Đã hoàn thành cho " << num_Custom << " khách hàng, x = " << x << ", j = " << jn << endl;
            }
        }
    }

    // freopen("D:\\Project GA new\\data\\Data_200_10_4.txt", "r", stdin);
    //             cin >> num_Custom;
    //             Cus.resize(num_Custom + 1);
    //             Cus[0] = Customer(0, INT_MAX, 0, 0);
    //             for (int i = 1; i <= num_Custom; i++)
    //             {
    //                 cin >> Cus[i].start >> Cus[i].end >> Cus[i].weight >> Cus[i].role;
    //             }

    //             Ex.resize(num_Custom + 1, vector<double>(num_Custom + 1));

    //             cin >> Max_Weight_Truck >> Max_Weight_Drone >> Max_Energy >> t_Truck >> t_drone >> k_Truck >> k_drone >> en_drone >> cost_truck >> cost_drone >> max_truck >> max_drone;

    //             for (int i = 0; i <= num_Custom; i++)
    //             {
    //                 for (int j = 0; j <= num_Custom; j++)
    //                     cin >> Ex[i][j];
    //             }

    //             for (int i = 1; i <= num_Custom; i++)
    //             {
    //                 cout << Cus[i].start << " " << Cus[i].end << " " << Cus[i].weight << " " << Cus[i].role << endl;
    //             }

    //             cout << Max_Weight_Truck << " " << Max_Weight_Drone << " " << Max_Energy << endl;
    //             cout << t_Truck << " " << t_drone << " " << k_Truck << " " << k_drone << " " << en_drone << " " << cost_truck << " " << cost_drone << " " << max_truck << " " << max_drone << endl;
    //             for (int i = 0; i <= num_Custom; i++)
    //             {
    //                 for (int j = 0; j <= num_Custom; j++)
    //                     cout << Ex[i][j] << " ";
    //                 cout << endl;
    //             }

    // while(max_truck >0){
    //     auto pop = Population(num_Custom, 50);
    //     pop.Genetic("NSGA_II", 0.4, 50, false);
    //     string file_name = "D:\\Project GA new\\multi_truck_problem\\pareto_front_"+to_string(max_truck)+".txt";
    //     printParetoFront(pareto_front_in_generation, file_name);
    //     cout << "Đã hoàn thành cho " << num_Custom << " khách hàng, max_truck = " << max_truck << endl;

    //     pareto_front_in_generation.clear();
    //     max_truck--;
    //     max_drone += 3;
    // }

    return 0;
}