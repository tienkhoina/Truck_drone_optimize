#include <iostream>
#include "function.h"
#include "Constants.h"
#include <set>


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