#include "Solution.h"
#include "Custom.h"
#include <algorithm>

Solution::Solution(vector<vector<int>> route, vector<int> role)
    : Route(route), Role(role)
{
    num_drone = 0;
    num_truck = 0;
    f1 = 0;
    f2 = 0;

    for (const vector<int> &Trip : Route)
    {
        f1 += Trip.size() - 2;
    }

    vector<pair<int, vector<int>>> combined;
    for (size_t i = 0; i < Role.size(); i++)
    {
    
        combined.push_back({Role[i], Route[i]});
    }
    sort(combined.begin(), combined.end());

    for (size_t i = 0; i < combined.size(); i++)
    {
        Role[i] = combined[i].first;
        Route[i] = combined[i].second;
    }
    int index = 0;

    // 🚛 Xử lý TRUCK (role = 0)
    while (index < Route.size() && Role[index] == 0)
    {
        double now = 0;
        double weight = 0;
        num_truck++;

        //cout << "\n🚛 Truck " << num_truck << " bắt đầu từ 0\n";

        for (int i = 0; i < Route[index].size() - 1; i++)
        {
            int from = Route[index][i];
            int to = Route[index][i + 1];

            now += t_Truck * Ex[from][to];
            weight += Cus[to].weight;
            f2 += k_Truck * Ex[from][to];

            // //cout << "👉 Từ " << from << " đến " << to << " t_truck: " << t_Truck << " Ex: " << Ex[from][to]
            //      << " | now = " << now
            //      << " | trọng lượng = " << weight << endl;

            if (now < Cus[to].start)
                now = Cus[to].start;
            else if (now > Cus[to].end)
            {
                //cout << "❌ Lỗi thời gian tại " << to << " | now = " << now << " > end = " << Cus[to].end << endl;
                f1 = -1;
                f2 = INT_MAX;
                return;
            }
        }

        if (weight > Max_Weight_Truck)
        {
            //cout << "❌ Lỗi trọng lượng: " << weight << " > " << Max_Weight_Truck << endl;
            f1 = -1;
            f2 = INT_MAX;
            return;
        }

        index++;
    }

    // 🚀 Xử lý DRONE (role > 0)
    int current_role = -1;
    double now = 0;
    double useEnergy = 0;

    while (index < Route.size())
    {
        double weight = 0;
        useEnergy = 0;

        if (current_role == Role[index])
        {
            //cout << "\n🚀 Drone tiếp tục sử dụng (role = " << Role[index] << ")\n";
        }
        else
        {
            now = 0;
            current_role = Role[index];
            num_drone++;
            //cout << "\n🚀 Drone " << num_drone << " bắt đầu từ 0 (role = " << current_role << ")\n";
        }

        for (int i = 0; i < Route[index].size() - 1; i++)
        {
            int from = Route[index][i];
            int to = Route[index][i + 1];

            now += t_drone * Ex[from][to];
            weight += Cus[to].weight;
            useEnergy += en_drone * Ex[from][to];
            f2 += k_drone * Ex[from][to];

            // //cout << "👉 Từ " << from << " đến " << to
            //      << " | now = " << now
            //      << " | trọng lượng = " << weight
            //      << " | năng lượng = " << useEnergy << endl;

            if (now < Cus[to].start)
                now = Cus[to].start;
            else if (now > Cus[to].end)
            {
                //cout << "❌ Lỗi thời gian tại " << to << " | now = " << now << " > end = " << Cus[to].end << endl;
                f1 = -1;
                f2 = INT_MAX;
                return;
            }
        }

        if (weight > Max_Weight_Drone)
        {
            //cout << "❌ Lỗi trọng lượng drone: " << weight << " > " << Max_Weight_Drone << endl;
            f1 = -1;
            f2 = INT_MAX;
            return;
        }
        if (useEnergy > Max_Energy)
        {
            //cout << "❌ Lỗi năng lượng drone: " << useEnergy << " > " << Max_Energy << endl;
            f1 = -1;
            f2 = INT_MAX;
            return;
        }

        index++;
    }

    f2 += cost_drone * num_drone + cost_truck * num_truck;
}

// Constructor sao chép
Solution::Solution(const Solution &other)
    : Route(other.Route), Role(other.Role), f1(other.f1), f2(other.f2),
      num_drone(other.num_drone), num_truck(other.num_truck) {}

// Toán tử gán
Solution &Solution::operator=(const Solution &other)
{
    if (this == &other)
        return *this;
    Route = other.Route;
    Role = other.Role;
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
    return Route == other.Route && Role == other.Role && f1 == other.f1 && f2 == other.f2 && num_drone == other.num_drone &&
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

