#include <iostream>
#include "function.h"
#include "Constants.h"
#include <set>
#include <fstream>
#include "TSPTW.h"


random_device rd;
mt19937 gen(rd());

int getRandomNumber(int a, int b)
{
    uniform_int_distribution<int> dist(a, b);
    return dist(gen);
}

double getRandomDouble(double a, double b)
{
    uniform_real_distribution<double> dist(a, b);
    return dist(gen);
}

bool checkInsertNext(vector<int> x, int v, int roleId, double now){
    vector<int> temp = x;
    double weight=0;
    temp.push_back(v);
    if(Cus[v].role == 1 && roleId!=0)
        return false;
    if(roleId == 0){
        for (int i = 0; i < temp.size()-1;i++){
            now += t_Truck * Ex[temp[i]][temp[i + 1]];
            if(now < Cus[temp[i+1]].start){
                now = Cus[temp[i + 1]].start;
            }
            else if (now > Cus[temp[i + 1]].end){
                return false;
            }
            weight += Cus[temp[i + 1]].weight;
        }
        if(weight > Max_Weight_Truck)
            return false;
    }
    else{
        double useEnergy = 0;
        for (int i = 0; i < temp.size() - 1; i++)
        {
            now += t_drone * Ex[temp[i]][temp[i + 1]];

            if (now < Cus[temp[i + 1]].start)
            {
                now = Cus[temp[i + 1]].start;
            }
            else if (now > Cus[temp[i + 1]].end)
            {
                return false;
            }
            weight += Cus[temp[i + 1]].weight;
            useEnergy += en_drone * Ex[temp[i]][temp[i + 1]];
        }
        if (weight > Max_Weight_Drone)
            return false;
        if (useEnergy>Max_Energy)
            return false;
    }
    return true;
}

bool checkValidVector(vector<int> x, int roleId, double now){
    vector<int> temp = x;
    double weight = 0;
    if (roleId == 0)
    {
        now = 0;
        for (int i = 0; i < temp.size() - 1; i++)
        {
            now += t_Truck * Ex[temp[i]][temp[i + 1]];
            if (now < Cus[temp[i + 1]].start)
            {
                now = Cus[temp[i + 1]].start;
            }
            else if (now > Cus[temp[i + 1]].end)
            {
                return false;
            }
            weight += Cus[temp[i + 1]].weight;
        }
        if (weight > Max_Weight_Truck)
            return false;
    }
    else
    {
        double useEnergy = 0;
        for (int i = 0; i < temp.size() - 1; i++)
        {
            if (Cus[temp[i + 1]].role==1)
                return false;
            now += t_drone * Ex[temp[i]][temp[i + 1]];

            if (now < Cus[temp[i + 1]].start)
            {
                now = Cus[temp[i + 1]].start;
            }
            else if (now > Cus[temp[i + 1]].end)
            {
                return false;
            }
            weight += Cus[temp[i + 1]].weight;
            useEnergy += en_drone * Ex[temp[i]][temp[i + 1]];
        }
        if (weight > Max_Weight_Drone)
            return false;
        if (useEnergy > Max_Energy)
            return false;
    }
    return true;
}

double getTimeDroneTrip(vector<int> x, double now){
    auto temp = x;
    for (int i = 0; i < x.size() - 1; i++){
        now += t_drone * Ex[temp[i]][temp[i + 1]];

        if (now < Cus[temp[i + 1]].start)
        {
            now = Cus[temp[i + 1]].start;
        }
    }

    return now;
}

set<pair<int, int>> extractEdges(const vector<vector<int>> &routes)
{
    set<pair<int, int>> edges;
    for (const auto &route : routes)
    {
        for (size_t i = 0; i < route.size() - 1; ++i)
        {
            edges.insert({route[i], route[i + 1]});
        }
    }
    return edges;
}

double jaccardSimilarity(const vector<vector<int>> &solutionA, const vector<vector<int>> &solutionB)
{
    set<pair<int, int>> edgesA = extractEdges(solutionA);
    set<pair<int, int>> edgesB = extractEdges(solutionB);

    set<pair<int, int>> intersectionSet;
    set<pair<int, int>> unionSet = edgesA;

    // Tính giao (intersection)
    for (const auto &edge : edgesB)
    {
        if (edgesA.count(edge))
        {
            intersectionSet.insert(edge);
        }
        unionSet.insert(edge); // Hợp (union)
    }

    // Tính chỉ số Jaccard
    double jaccardIndex = (double)intersectionSet.size() / unionSet.size();
    return jaccardIndex;
}
pair<int,vector<int>> solverTSPTWmapping(const vector<int> &Trip, const vector<vector<double>> &ex, double t_trip,double start_time){
    ////////cout << "bắt đầu giải" << endl;
    int size = Trip.size() - 1;
    //////cout << size << endl;
    vector<int> mapping(size);
    vector<vector<double>> map_cost(size, vector<double>(size));
    vector<double> earliest(size);
    vector<double> lastest(size);
    vector<double> service_time(size, 0);

    for (int i = 0; i < size ; i++){
        
        mapping[i] = Trip[i];
        //////cout << mapping[i];
    }
    //////cout << endl;

    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j <size; j++)
        {
            map_cost[i][j] = t_trip * ex[mapping[i]][mapping[j]];
            //////cout << map_cost[i][j] << " ";
        }
        //////cout << endl;
    }
    
    for (int i = 0; i < size; i++){
        earliest[i] = Cus[mapping[i]].start;
        lastest[i] = Cus[mapping[i]].end;
        //////cout << earliest[i] <<" "<< lastest[i] << endl;
    }

    TSPTW tsptw(size-1, map_cost, earliest, lastest, service_time, start_time);
    ////////cout << "tạo xong đối tượng";
    auto solution = tsptw.solve();
    if(solution.first != -1){
        vector<int> ReturnTrip;
        for(int x: solution.second){
            ReturnTrip.push_back(mapping[x]);
        }
        return {solution.first, ReturnTrip};
    }
    else{
        for (int i = 0; i < size; i++)
        {
            cout << i << " " << mapping[i] << endl;
        }
        for (int i = 0; i < size; i++)
        {
            cout << earliest[i] <<" "<< lastest[i] << endl;
        }

        for (int i = 0; i < size; i++)
        {
            for (int j = 0; j < size; j++)
            {
               
                cout << map_cost[i][j] << " ";
            }
            cout << endl;
        }
        return {-1, {}};
    }  
}

void printParetoFront(vector<vector<pair<int, double>>> &pr1, string file_path)
{
    ofstream file(file_path);
    if (!file)
    {
        cerr << "Không thể mở file: " << file_path << endl;
        return;
    }

    // In dữ liệu theo đúng định dạng yêu cầu
    for (size_t i = 0; i < pr1.size(); ++i)
    {
        file<<"Thế hệ thứ: "<<"\n" << i << "\n"; // In chỉ mục của vector
        for (const auto &[first, second] : pr1[i])
        {
            file << first << " " << second << "\n";
        }
    }

    file.close();
}

bool checkValidSolution(vector<vector<int>> route, vector<int> role){
    int n_truck=0;
    int n_drone=0;

    int index = 0;

    // 🚛 Xử lý TRUCK (role = 0)
    while (index < route.size() && role[index] == 0)
    {
        n_truck++;
        
        if(!checkValidVector(route[index],role[index],0))
            return false;
        index++;
    }

    int current_role = -1;
    double now = 0;

    while (index < route.size())
    {
        double weight = 0;
        double useEnergy = 0;

        if (current_role == role[index])
        {
            now = getTimeDroneTrip(route[index - 1], now);
        }
        else
        {
            current_role = role[index];
            now = 0;
            n_drone++;
            // cout << "\n🚀 Drone " << num_drone << " bắt đầu từ 0 (role = " << current_role << ")\n";
        }
        if (!checkValidVector(route[index], role[index], now))
            return false;
        index++;
    }
    if(n_truck>max_truck||n_drone>max_drone)
        return false;

    return true;
}
