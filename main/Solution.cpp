#include "Solution.h"
#include "Custom.h"
#include <algorithm>
#include "TSPTW.h"
#include "function.h"
#include <algorithm> // nhớ include nếu chưa có

Solution::Solution(vector<vector<int>> route, vector<int> role, vector<int> mark)
    : Route(route), Role(role), Mark(mark), num_drone(0), num_truck(0), f1(0), f2(0)
{
    
    // Tính số điểm phục vụ (bỏ điểm depot ở đầu và cuối)
    for (const auto &trip : Route)
        f1 += trip.size() - 2;

    // Sắp xếp Role, Route, Mark theo Role

    vector<tuple<int, vector<int>, int>> combined;
    for (size_t i = 0; i < Role.size(); ++i)
        combined.emplace_back(Role[i], Route[i], Mark[i]);

    // Dùng stable_sort để giữ thứ tự ban đầu khi Role giống nhau
    stable_sort(combined.begin(), combined.end(),
                [](const tuple<int, vector<int>, int> &a, const tuple<int, vector<int>, int> &b)
                {
                    return get<0>(a) < get<0>(b);
                });

    for (size_t i = 0; i < combined.size(); ++i)
    {
        Role[i] = get<0>(combined[i]);
        Route[i] = get<1>(combined[i]);
        Mark[i] = get<2>(combined[i]);
    }

    int index = 0;

    // ===== XỬ LÝ TRUCK (role = 0) =====
    while (index < Route.size() && Role[index] == 0)
    {
        if (!checkValidVector(Route[index], 0, 0))
        {
            cout << "❌ Lỗi Truck không tìm được tối ưu\n";
            invalidate();
            return;
        }


        double weight = 0;
        for (int i = 0; i < Route[index].size() - 1; ++i)
            weight += Cus[Route[index][i + 1]].weight;

        if (weight > Max_Weight_Truck)
        {
            cout << "❌ Lỗi trọng lượng Truck\n";
            invalidate();
            return;
        }

        ++num_truck;
        ++index;
    }
  

    // ===== XỬ LÝ DRONE (role > 0) =====
    double currentTime = 0;
    int currentRole = -1;

    while (index < Route.size())
    {
        
        double weight = 0;
        if (currentRole != Role[index])
        {
            currentRole = Role[index];
            ++num_drone;
            currentTime = 0;
        
        }

        if (!checkValidVector(Route[index], currentRole, currentTime))
        {
            // cout << "❌ Lỗi Drone không tìm được tối ưu\n";
            invalidate();
            return;
        }

        for (int i = 0; i < Route[index].size() - 1; ++i)
            weight += Cus[Route[index][i + 1]].weight;

            
        if (weight > Max_Weight_Drone)
        {
            cout << "❌ Lỗi trọng lượng Drone\n";
            invalidate();
            return;
        }
        currentTime = getTimeDroneTrip(Route[index], currentTime);
        ++index;
    }
    

    // ===== TÍNH CHI PHÍ DI CHUYỂN (TRUCK) =====
    index = 0;
    while (index < Route.size() && Role[index] == 0)
    {
        for (int i = 0; i < Route[index].size() - 1; ++i)
            f2 += k_Truck * Ex[Route[index][i]][Route[index][i + 1]];
        ++index;
    }

    // ===== TÍNH CHI PHÍ DI CHUYỂN (DRONE) & ENERGY =====
    currentRole = -1;
    while (index < Route.size())
    {
        double energy = 0;
        if (currentRole != Role[index])
            currentRole = Role[index];

        for (int i = 0; i < Route[index].size() - 1; ++i)
        {
            int from = Route[index][i];
            int to = Route[index][i + 1];
            double dist = Ex[from][to];
            energy += en_drone * dist;
            f2 += k_drone * dist;
        }

        if (energy > Max_Energy)
        {
            cout << "❌ Lỗi năng lượng Drone" << energy << endl;
            cout << checkValidVector(Route[index], Role[index], 0) << endl;
            invalidate();
            return;
        }

        ++index;
    }

    // ===== KIỂM TRA SỐ LƯỢNG PHƯƠNG TIỆN =====
    if (num_drone > max_drone || num_truck > max_truck)
    {
        cout << "❌ Quá số lượng Drone/Truck\n";
        invalidate();
        return;
    }

    f2 += cost_drone * num_drone + cost_truck * num_truck;
}
void Solution::optimize_trip(){
    //lưu giá trị cũ nếu không tối ưu được
    vector<vector<int>> oldRoute = Route;
    vector<int> oldRole = Role;
    vector<int> oldMark = Mark;
    double oldf1 = f1;
    double oldf2 = f2;
    int old_num_drone = num_drone;
    int old_num_truck = num_truck;

    num_drone = 0;
    num_truck = 0;
    f2 = 0;

    int index = 0;

    // ===== TRUCK =====
    while (index < Route.size() && Role[index] == 0)
    {
        if (!checkValidVector(Route[index], 0, 0))
        {
            cout << "❌ Lỗi Truck không hợp lệ\n";
            invalidate();
            return;
        }

        if (Mark[index] == 1)
        {
            ++index;
            continue;
        }

        int batch_size = 10;
        auto result = (Route[index].size() - 1 <= batch_size)
                          ? solverTSPTWmapping(Route[index], Ex, t_Truck, 0)
                          : solverTSPTWmappingLarge(Route[index], Ex, t_Truck, 0, batch_size);

        if (result.first == -1)
        {
            if (!checkValidVector(Route[index], 0, 0))
            {
                cout << "❌ Truck không tìm được tối ưu\n";
                Route = oldRoute;
                Role = oldRole;
                Mark= oldMark;
                f1 = oldf1;
                f2 = oldf2;
                return;
            }
            ++index;
            continue;
        }

        Route[index] = result.second;
        if(!checkValidVector(Route[index], 0, 0)){
            Route = oldRoute;
            Role = oldRole;
            Mark= oldMark;
            f1 = oldf1;
            f2 = oldf2;
            return;
        }
        Mark[index] = 1;

        double weight = 0;
        for (int i = 0; i < Route[index].size() - 1; ++i)
            weight += Cus[Route[index][i + 1]].weight;

        if (weight > Max_Weight_Truck)
        {
            cout << "❌ Trọng lượng Truck vượt quá giới hạn\n";
            invalidate();
            return;
        }

        ++num_truck;
        ++index;
    }

    // ===== DRONE =====
    double currentTime = 0;
    int currentRole = -1;

    while (index < Route.size())
    {
        double weight = 0;
        if (currentRole != Role[index])
        {
            currentRole = Role[index];
            ++num_drone;
            currentTime = 0;
        }

        for (int i = 0; i < Route[index].size() - 1; ++i)
            weight += Cus[Route[index][i + 1]].weight;

        if (weight > Max_Weight_Drone)
        {
            cout << "❌ Trọng lượng Drone vượt quá giới hạn\n";
            invalidate();
            return;
        }

        if (Mark[index] == 1)
        {
            currentTime = getTimeDroneTrip(Route[index], currentTime);
            ++index;
            continue;
        }

        int batch_size = 10;
        auto result = (Route[index].size() - 1 <= batch_size)
                          ? solverTSPTWmapping(Route[index], Ex, t_drone, currentTime)
                          : solverTSPTWmappingLarge(Route[index], Ex, t_drone, currentTime, batch_size);

        if (result.first == -1)
        {
            if (!checkValidVector(Route[index], Role[index], currentTime))
            {
                Route = oldRoute;
                Role = oldRole;
                Mark = oldMark;
                f1 = oldf1;
                f2 = oldf2;
                return;
            }
            currentTime = getTimeDroneTrip(Route[index], currentTime);
            ++index;
            continue;
        }

        Route[index] = result.second;
        if(!checkValidVector(Route[index], Role[index], currentTime)){
            Route = oldRoute;
            Role = oldRole;
            Mark= oldMark;
            f1 = oldf1;
            f2 = oldf2;
            return;
        }
        Mark[index] = 1;
        currentTime = getTimeDroneTrip(Route[index], currentTime);
        ++index;
    }

    // ===== TÍNH CHI PHÍ =====
    f2 = 0;
    index = 0;
    while (index < Route.size() && Role[index] == 0)
    {
        for (int i = 0; i < Route[index].size() - 1; ++i)
            f2 += k_Truck * Ex[Route[index][i]][Route[index][i + 1]];
        ++index;
    }

    currentRole = -1;
    while (index < Route.size())
    {
        double energy = 0;
        if (currentRole != Role[index])
            currentRole = Role[index];

        for (int i = 0; i < Route[index].size() - 1; ++i)
        {
            int from = Route[index][i];
            int to = Route[index][i + 1];
            double dist = Ex[from][to];
            energy += en_drone * dist;
            f2 += k_drone * dist;
        }

        if (energy > Max_Energy)
        {
            cout << "❌ Drone vượt quá giới hạn năng lượng\n";
            invalidate();
            return;
        }

        ++index;
    }

    if (num_drone > max_drone || num_truck > max_truck)
    {
        cout << "❌ Vượt quá số lượng Drone/Truck\n";
        invalidate();
        return;
    }
    num_truck = old_num_truck;
    num_drone = old_num_drone;
    f2 += cost_drone * old_num_drone + cost_truck * old_num_truck;
}

void Solution::invalidate()
{
    f1 = -1;
    f2 = INT_MAX;
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
    // Chỉ thống trị nếu nó không kém hơn về mọi mặt và tốt hơn ít nhất một mặt
    return (f1 >= other.f1 && f2 <= other.f2 && (f1 > other.f1 || f2 < other.f2));
}

bool Solution::operator==(const Solution &other) const
{
    return f1 == other.f1 &&
           abs(f2 - other.f2) < 1e-6 && // Tránh lỗi số thực
           num_drone == other.num_drone &&
           num_truck == other.num_truck;
}

// In kết quả
void Solution::print() const
{
    if (f1 == -1 || f2 == INT_MAX)
    {
        cout << "Not feasible";
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


void Solution::Tabu_Optimize(int max_iter, int tabu_tenure)
{
    // Khởi tạo danh sách Tabu: [customer][route] -> (tabu_flag, tenure)
    vector<vector<pair<int, int>>> tabu_list(num_Custom + 1, vector<pair<int, int>>(Route.size(), {0, 0}));

    int iter = 0;
    double best_f2 = f2;
    Solution best_solution = *this;

    cout << "Bắt đầu Tabu Search\n";

    while (iter < max_iter)
    {
     
        vector<Solution> neighbors;
        vector<tuple<int, int, int>> moves; // (customer, old_route, new_route)

        // Duyệt qua từng khách hàng trong các route
        for (int i = 0; i < Route.size(); i++)
        {
    
            if(Route[i].size() <= 3)
                continue; 
            for (int p = 1; p < Route[i].size() - 1; p++) // Bỏ depot
            {
                int customer = Route[i][p];

                for (int j = 0; j < Route.size(); j++)
                {
                    if (i == j)
                        continue; // Không tự chuyển trong cùng 1 route

                    bool inserted = false;
                    for (int pos = 1; pos < Route[j].size()-1; pos++) // thử tất cả vị trí chèn
                    {
                        // Copy hiện tại
                        auto neighbor_route = Route;
                        auto neighbor_role = Role;
                        auto neighbor_mark = Mark;

                        // Xóa khách khỏi route i
                        neighbor_route[i].erase(neighbor_route[i].begin() + p);

                        // Chèn vào route j tại vị trí pos
                        neighbor_route[j].insert(neighbor_route[j].begin() + pos, customer);

                        // Đánh dấu lại mark
                        neighbor_mark[i] = 0;
                        neighbor_mark[j] = 0;

                        // Check tính hợp lệ
                        if (checkValidSolution(neighbor_route, neighbor_role))
                        {
                          
                            Solution neighbor(neighbor_route, neighbor_role, neighbor_mark);
                            neighbor.optimize_trip();

                            if (neighbor.f1 != -1 && neighbor.f2 != INT_MAX)
                            {
                                bool is_tabu = tabu_list[customer][j].first == 1;
                                bool aspiration = neighbor.f2 < best_f2;

                                if (!is_tabu || aspiration)
                                {
                                    neighbors.push_back(neighbor);
                                    moves.emplace_back(customer, i, j);
                                }
                            }
                            inserted = true;
                            break; // Chỉ cần 1 chỗ chèn hợp lệ là đủ
                        }
                    }
                }
            }
        }


        if (neighbors.empty())
            break; // Không sinh được hàng xóm nào hợp lệ

        // Chọn neighbor tốt nhất
        int best_idx = 0;
        for (int i = 1; i < neighbors.size(); i++)
        {
            if (neighbors[i] < neighbors[best_idx])
                best_idx = i;
        }

        Solution best_neighbor = neighbors[best_idx];
        int customer = get<0>(moves[best_idx]);
        int old_route = get<1>(moves[best_idx]);
        int new_route = get<2>(moves[best_idx]);

        // Update Tabu List
        tabu_list[customer][new_route] = {1, tabu_tenure};

        // Move đến neighbor
        *this = best_neighbor;

        // Update best toàn cục
        if (f2 < best_f2)
        {
            best_f2 = f2;
            best_solution = *this;
        }

        // Giảm tenure Tabu List
        for (auto &row : tabu_list)
        {
            for (auto &entry : row)
            {
                if (entry.second > 0)
                {
                    entry.second--;
                    if (entry.second == 0)
                        entry.first = 0;
                }
            }
        }

        iter++;
    }

    // Trả lại best solution tìm được
    *this = best_solution;
}

void Solution::printToFile(const string &filename) const
{
    ofstream out(filename, ios::app); // mở file ở chế độ append

    if (!out)
    {
        cerr << "Không thể mở file: " << filename << endl;
        return;
    }

    if (f1 == -1 || f2 == INT_MAX)
    {
        out << "Not feasible\n";
        int index = 0;
        for (const vector<int> &Trip : Route)
        {
            for (int x : Trip)
            {
                out << x << " ";
            }
            out << "Role: " << Role[index++] << "\n";
        }
        out << "\n";
    }
    else
    {
        out << "Số khách: " << f1 << "\n";
        out << "Chi phí: " << f2 << "\n";
        int index = 0;
        for (const vector<int> &Trip : Route)
        {
            for (int x : Trip)
            {
                out << x << " ";
            }
            out << "Role: " << Role[index++] << "\n";
        }
        out << "\n";
    }

    out.close(); // đóng file sau khi ghi xong
}
