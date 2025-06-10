#include "constants.h"

// Định nghĩa biến toàn cục
double Max_Energy;
double Max_Weight_Drone;
double Max_Weight_Truck;
double k_Truck ;
double k_drone;
double t_Truck;
double t_drone;
double en_drone;
vector<vector<double>> Ex;
vector<Customer> Cus;
int num_Custom;
double cost_drone;
double cost_truck;
int min_custom_serve;
int max_truck;
int max_drone;
double drop_insert_custom=0.2;
double drop_insert_trip=0.2;
vector<vector<pair<int, double>>> pareto_front_in_generation;