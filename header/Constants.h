#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "Custom.h"
#include <iostream>
#include <vector>

using namespace std;


extern double Max_Energy; //giới hạn năng lượng
extern double Max_Weight_Drone; // giới hạn khối lượng
extern double Max_Weight_Truck;
extern double k_Truck; //hệ số chi phí
extern double k_drone;
extern double t_Truck;// hệ số thời gian đường đi
extern double t_drone;
extern double en_drone;// hệ số năng lượng đường đi
extern vector<vector<double>> Ex;// khoảng cách
extern vector<Customer> Cus;
extern int num_Custom;
extern double cost_drone;//chi phí thuê
extern double cost_truck;
extern int min_custom_serve; // đánh giá số khách hàng ít nhất cần được phục vụ
extern int max_truck; // giới hạn số lượng xe tải
extern int max_drone; // giới hạn số lượng drone
extern double drop_insert_custom;// tỉ lệ drop out khi thêm khách hàng
extern double drop_insert_trip;// tỉ lệ drop out khi thêm đường đi
extern vector<vector<pair<int, double>>> pareto_front_in_generation;

#endif // CONSTANTS_H
