#include "Solution.h"
#include "Custom.h"
#include <algorithm>
#include "TSPTW.h"
#include "function.h"

Solution::Solution(vector<vector<int>> route, vector<int> role, vector<int> mark) : Route(route), Role(role),Mark(mark)
{
    //cout << "bắt đầu tạo solution" << endl;
    num_drone = 0;
    num_truck = 0;
    f1 = 0;
    f2 = 0;

    for (const vector<int> &Trip : Route)
    {
        f1 += Trip.size() - 2;
    }

    vector<tuple<int, vector<int>, int>> combined; // Dùng tuple để chứa cả Role, Route và Mark

    // Gộp dữ liệu vào combined
    for (size_t i = 0; i < Role.size(); i++)
    {
        combined.push_back({Role[i], Route[i], Mark[i]});
    }

    // Sắp xếp theo Role
    sort(combined.begin(), combined.end());

    // Cập nhật lại Role, Route và Mark theo thứ tự mới
    for (size_t i = 0; i < combined.size(); i++)
    {
        Role[i] = get<0>(combined[i]);
        Route[i] = get<1>(combined[i]);
        Mark[i] = get<2>(combined[i]);
    }

    int index = 0;

    // 🚛 Xử lý TRUCK (role = 0)
    while (index < Route.size() && Role[index] == 0)
    {
        double weight = 0;
        num_truck++;

        // cout << "\n🚛 Truck " << num_truck << " bắt đầu từ 0\n";

        for (int i = 0; i < Route[index].size() - 1; i++)
        {
            int from = Route[index][i];
            int to = Route[index][i + 1];

            weight += Cus[to].weight;
        }

        if (weight > Max_Weight_Truck)
        {
            cout << "❌ Lỗi trọng lượng: " << weight << " > " << Max_Weight_Truck << endl;
            f1 = -1;
            f2 = INT_MAX;
            return;
        }

        index++;
    }
    int current_role = -1;

    while (index < Route.size())
    {
        double weight = 0;

        if (current_role == Role[index])
        {
            // cout << "\n🚀 Drone tiếp tục sử dụng (role = " << Role[index] << ")\n";
        }
        else
        {
            current_role = Role[index];
            num_drone++;
            // cout << "\n🚀 Drone " << num_drone << " bắt đầu từ 0 (role = " << current_role << ")\n";
        }

        for (int i = 0; i < Route[index].size() - 1; i++)
        {
            int from = Route[index][i];
            int to = Route[index][i + 1];

            weight += Cus[to].weight;

        }

        if (weight > Max_Weight_Drone)
        {
            // cout << "❌ Lỗi trọng lượng drone: " << weight << " > " << Max_Weight_Drone << endl;
            f1 = -1;
            f2 = INT_MAX;
            return;
        }
        index++;
    }
    //cout << "kiểm tra xong weight" << endl;

    index = 0;
    while (index < Route.size() && Role[index] == 0)
    {
        //cout << Mark[index] << endl;
        if(Mark[index]==1) {
            index++;
            continue;
        }
        auto solut = solverTSPTWmapping(Route[index], Ex, k_Truck, 0.0);
        //cout << "solution " << solut.first << endl;
        if(solut.first == -1){
            // cout << " không tìm được tối ưu" << endl;
            f1 = -1;
            f2 = INT_MAX;
            return;
        }
        else{
            Route[index] = solut.second;
            Mark[index] = 1;
            index++;
        }
    }

    double now = 0;
    current_role = -1;
    while (index < Route.size())
    {
        if (current_role == Role[index])
        {
            // cout << "\n🚀 Drone tiếp tục sử dụng (role = " << Role[index] << ")\n";
        }
        else
        {
            now = 0;
            current_role = Role[index];
        }

        if(Mark[index]==1){
            now = getTimeDroneTrip(Route[index], now);
            index++;
            continue;
        }
        else{
            auto solut = solverTSPTWmapping(Route[index], Ex, k_drone, now);
            if(solut.first == -1){
                if(now ==0)
                    cout << " không tìm được tối ưu" << endl;
                f1 = -1;
                f2 = INT_MAX;
                return;
            }
            else{
                Route[index] = solut.second;
                now = getTimeDroneTrip(Route[index], now);
                Mark[index] = 1;
                index++;
            }
        }
    }

    //cout << "tối ưu xong trip " << endl;

    index = 0;

    // 🚛 Xử lý TRUCK (role = 0)
    while (index < Route.size() && Role[index] == 0)
    {
        double now = 0;

        // cout << "\n🚛 Truck " << num_truck << " bắt đầu từ 0\n";

        for (int i = 0; i < Route[index].size() - 1; i++)
        {
            int from = Route[index][i];
            int to = Route[index][i + 1];
            f2 += k_Truck * Ex[from][to];

            // //cout << "👉 Từ " << from << " đến " << to << " t_truck: " << t_Truck << " Ex: " << Ex[from][to]
            //      << " | now = " << now
            //      << " | trọng lượng = " << weight << endl;
        }
        index++;
    }

    // 🚀 Xử lý DRONE (role > 0)
    current_role = -1;
    double useEnergy = 0;

    while (index < Route.size())
    {
        useEnergy = 0;

        if (current_role == Role[index])
        {
            // cout << "\n🚀 Drone tiếp tục sử dụng (role = " << Role[index] << ")\n";
        }
        else
        {
            current_role = Role[index];
            // cout << "\n🚀 Drone " << num_drone << " bắt đầu từ 0 (role = " << current_role << ")\n";
        }

        for (int i = 0; i < Route[index].size() - 1; i++)
        {
            int from = Route[index][i];
            int to = Route[index][i + 1];

            useEnergy += en_drone * Ex[from][to];
            f2 += k_drone * Ex[from][to];

            // //cout << "👉 Từ " << from << " đến " << to
            //      << " | now = " << now
            //      << " | trọng lượng = " << weight
            //      << " | năng lượng = " << useEnergy << endl;
        }

        if (useEnergy > Max_Energy)
        {
            cout << "❌ Lỗi năng lượng drone: " << useEnergy << " > " << Max_Energy << endl;
            f1 = -1;
            f2 = INT_MAX;
            return;
        }

        index++;
    }

    //cout << " kiểm tra xong energy" << endl;

    if (num_drone > max_drone || num_truck > max_truck)
    {
        cout << "quá số lượng" << endl;;
        f1 = -1;
        f2 = INT_MAX;
        return;
    }

    f2 += cost_drone * num_drone + cost_truck * num_truck;
}


// Constructor sao chép
Solution::Solution(const Solution &other)
    : Route(other.Route), Role(other.Role), Mark(other.Mark), // Thêm Mark
      f1(other.f1), f2(other.f2), num_drone(other.num_drone), num_truck(other.num_truck)
{
}

Solution::Solution(vector<vector<int>> route, vector<int> role)
    : Solution(route, role, vector<int>(route.size(), 0)) {}

// Toán tử gán
Solution &Solution::operator=(const Solution &other)
{
    if (this == &other)
        return *this;
    Route = other.Route;
    Role = other.Role;
    Mark = other.Mark;
    f1 = other.f1;
    f2 = other.f2;
    num_drone = other.num_drone;
    num_truck = other.num_truck;
    return *this;
}

// So sánh nghiệm
bool Solution::operator<(const Solution &other) const
{
    bool better_in_one = (f1 > other.f1) || (f2 < other.f2);
    bool not_worse_in_all = (f1 >= other.f1) && (f2 <= other.f2);
    return not_worse_in_all && better_in_one;
}

bool Solution::operator==(const Solution &other) const{
    return Route == other.Route && Role == other.Role && Mark == other.Mark && f1 == other.f1 && f2 == other.f2 && num_drone == other.num_drone &&
           num_truck == other.num_truck;
}

// In kết quả
void Solution::print()
{
    if (f1 == -1 || f2 == INT_MAX)
    {
        cout << "Not feasible";
    }
    else
    {
        cout << "Số khách: " << f1 << endl;
        cout << "Chi phí: " << f2 << endl;
        int index = 0;
        for (vector<int> Trip : Route)
        {
            for (int x : Trip)
            {
                cout << x << " ";
            }
            cout << "Role: " << Role[index++] << endl;
        }
        cout << endl;
    }
}

