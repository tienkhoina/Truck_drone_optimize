#include <iostream>
#include "function.h"
#include "Constants.h"
#include <set>
#include <fstream>
#include "TSPTW.h"
#include "Hamilton_path.h"

random_device rd;
mt19937 gen(rd()); // Khởi tạo bộ sinh số ngẫu nhiên

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

bool checkValidVector(vector<int> x, int roleId, double now)
{
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
            if (Cus[temp[i + 1]].role == 1)
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
bool checkInsertNext(vector<int> x, int v, int roleId, double now)
{
    // Tạo bản sao của vector và thêm phần tử mới
    vector<int> temp = x;
    temp.push_back(v);
    temp.push_back(0); // Thêm depot vào cuối vector

    // double weight = 0;
    // double useEnergy = 0;

    // // Kiểm tra roleId
    // if (Cus[v].role == 1 && roleId != 0)
    //     return false;

    // // Trường hợp Truck (roleId == 0)
    // if (roleId == 0)
    // {
    //     for (int i = 0; i < temp.size() - 1; i++)
    //     {
    //         now += t_Truck * Ex[temp[i]][temp[i + 1]];
    //         if (now < Cus[temp[i + 1]].start)
    //         {
    //             now = Cus[temp[i + 1]].start;
    //         }
    //         else if (now > Cus[temp[i + 1]].end)
    //         {
    //             return false;
    //         }
    //         weight += Cus[temp[i + 1]].weight;
    //     }
    //     if (weight > Max_Weight_Truck)
    //         return false;
    // }
    // // Trường hợp Drone (roleId != 0)
    // else
    // {
    //     for (int i = 0; i < temp.size() - 1; i++)
    //     {
    //         if (Cus[temp[i + 1]].role == 1)
    //             return false;

    //         now += t_drone * Ex[temp[i]][temp[i + 1]];
    //         if (now < Cus[temp[i + 1]].start)
    //         {
    //             now = Cus[temp[i + 1]].start;
    //         }
    //         else if (now > Cus[temp[i + 1]].end)
    //         {
    //             return false;
    //         }
    //         weight += Cus[temp[i + 1]].weight;
    //         useEnergy += en_drone * Ex[temp[i]][temp[i + 1]];
    //     }
    //     if (weight > Max_Weight_Drone)
    //         return false;
    //     if (useEnergy > Max_Energy)
    //         return false;
    // }

    // return true;
    return checkValidVector(temp, roleId, now);
}

double getTimeDroneTrip(vector<int> x, double now)
{
    auto temp = x;
    for (int i = 0; i < x.size() - 1; i++)
    {
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
pair<int, vector<int>> solverTSPTWmapping(const vector<int> &Trip, const vector<vector<double>> &ex, double t_trip, double start_time)
{
    ////////cout << "bắt đầu giải" << endl;
    int size = Trip.size() - 1;
    //////cout << size << endl;
    vector<int> mapping(size);
    vector<vector<double>> map_cost(size, vector<double>(size));
    vector<double> earliest(size);
    vector<double> lastest(size);
    vector<double> service_time(size, 0);

    for (int i = 0; i < size; i++)
    {

        mapping[i] = Trip[i];
        //////cout << mapping[i];
    }
    //////cout << endl;

    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            map_cost[i][j] = t_trip * ex[mapping[i]][mapping[j]];
            //////cout << map_cost[i][j] << " ";
        }
        //////cout << endl;
    }

    for (int i = 0; i < size; i++)
    {
        earliest[i] = Cus[mapping[i]].start;
        lastest[i] = Cus[mapping[i]].end;
        //////cout << earliest[i] <<" "<< lastest[i] << endl;
    }

    TSPTW tsptw(size - 1, map_cost, earliest, lastest, service_time, start_time);
    ////////cout << "tạo xong đối tượng";
    auto solution = tsptw.solve();
    if (solution.first != -1)
    {
        vector<int> ReturnTrip;
        for (int x : solution.second)
        {
            ReturnTrip.push_back(mapping[x]);
        }
        return {solution.first, ReturnTrip};
    }
    else
    {
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
        file << "Thế hệ thứ: " << i << "\n"; // In chỉ mục của vector
        for (const auto &[first, second] : pr1[i])
        {
            file << first << " " << second << "\n";
        }
    }

    file.close();
}

bool checkValidSolution(vector<vector<int>> route, vector<int> role)
{
    int n_truck = 0;
    int n_drone = 0;

    int index = 0;

    // 🚛 Xử lý TRUCK (role = 0)
    while (index < route.size() && role[index] == 0)
    {
        n_truck++;

        if (!checkValidVector(route[index], role[index], 0))
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
    if (n_truck > max_truck || n_drone > max_drone)
        return false;

    return true;
}
pair<int, vector<int>> solverTSPTWmappingLarge(const vector<int> &Trip, const vector<vector<double>> &Ex, double t_trip, double start_time, int batch_size = 10)
{
    double now = start_time;
    int size = Trip.size() - 1;
    int num_batch = size / batch_size;
    int batch_size_last = size % batch_size;
    vector<int> solut;

    for (int i = 0; i < num_batch; i++)
    {
        int start = i * batch_size;
        int end = start + batch_size - 1;
        int batch_len = end - start + 1;

        vector<int> mapping(batch_len);
        for (int j = 0; j < batch_len; j++)
            mapping[j] = Trip[start + j];

        // DEBUG: In mapping của batch
        // cout << "Batch " << i + 1 << " (original nodes): ";
        // for (int m : mapping)
        //     cout << m << " ";
        // cout << endl;

        vector<vector<double>> map_cost(batch_len, vector<double>(batch_len));
        for (int j = 0; j < batch_len; j++)
            for (int k = 0; k < batch_len; k++)
                map_cost[j][k] = t_trip * Ex[mapping[j]][mapping[k]];

        vector<double> earliest(batch_len), latest(batch_len), service_time(batch_len, 0);
        for (int j = 0; j < batch_len; j++)
        {
            earliest[j] = Cus[mapping[j]].start;
            latest[j] = Cus[mapping[j]].end;
        }

        HamiltonPath tsptw(batch_len - 1, map_cost, earliest, latest, service_time, now);
        auto result = tsptw.solve();

        if (result.first == -1)
            return {-1, {}};

        // DEBUG: In lời giải sau khi solve
        // cout << "Batch " << i + 1 << " (solved order): ";
        // for (int idx : result.second)
        //     cout << mapping[idx] << " ";
        // cout << endl;

        for (int k = 0; k < result.second.size(); ++k)
        {
            int idx = result.second[k];
            int original_node = mapping[idx];
            solut.push_back(original_node);

            if (k + 1 < result.second.size())
            {
                int next_node = mapping[result.second[k + 1]];
                now += t_trip * Ex[original_node][next_node];
            }
        }
    }

    if (batch_size_last > 1)
    {
        int start = size - batch_size_last;
        int end = size-1;
        int len = end - start + 1;

        vector<int> mapping(len);
        for (int j = 0; j < len; j++)
            mapping[j] = Trip[start + j];

        // cout << "Final batch (original nodes): ";
        // for (int m : mapping)
        //     cout << m << " ";
        // cout << endl;

        vector<vector<double>> map_cost(len, vector<double>(len));
        for (int j = 0; j < len; j++)
            for (int k = 0; k < len; k++)
                map_cost[j][k] = t_trip * Ex[mapping[j]][mapping[k]];

        vector<double> earliest(len), latest(len), service_time(len, 0);
        for (int j = 0; j < len; j++)
        {
            earliest[j] = Cus[mapping[j]].start;
            latest[j] = Cus[mapping[j]].end;
        }

        HamiltonPath tsptw(len - 1, map_cost, earliest, latest, service_time, now);
        auto result = tsptw.solve();

        if (result.first == -1)
            return {-1, {}};

        // cout << "Final batch (solved order): ";
        // for (int idx : result.second)
        //     cout << mapping[idx] << " ";
        // cout << endl;

        for (int k = 0; k < result.second.size(); ++k)
        {
            int idx = result.second[k];
            int original_node = mapping[idx];
            solut.push_back(original_node);

            if (k + 1 < result.second.size())
            {
                int next_node = mapping[result.second[k + 1]];
                now += t_trip * Ex[original_node][next_node];
            }
        }
    }
    else if (batch_size_last == 1)
    {
        solut.push_back(Trip[size - 1]);
    }

    solut.push_back(Trip.back()); // Thêm depot cuối
    return {0, solut};
}
