#include "function.h"
#include "Population.h"
#include "NSGA.h"
#include "MOEA.h"
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <algorithm> // Để xáo trộn ngẫu nhiên

Population::Population(int _n, int _size) : n(_n), size(_size)
{
    set<Solution> inserted;
    int n_random_shuffle = size;
    cout << "Tạo cá thể bởi RD: " << n_random_shuffle << endl;

    while (inserted.size() < n_random_shuffle)
    {
        vector<vector<int>> Route;  // Danh sách các trip
        vector<int> Role;           // Role tương ứng với từng trip
        unordered_set<int> S_all;   // Tất cả khách hàng
        unordered_set<int> S_role1; // Chỉ lưu khách có role = 1

        // **1️⃣ Khởi tạo danh sách khách hàng**
        for (int j = 1; j <= n; j++)
        {
            S_all.insert(j);
            if (Cus[j].role == 1)
                S_role1.insert(j);
        }

        // **2️⃣ Truck phục vụ ngẫu nhiên khách hàng trước**
        while (!S_role1.empty()) // Lặp đến khi không còn khách role = 1
        {

            vector<int> route = {0}; // Bắt đầu từ kho
            double now = 0;          // Truck luôn bắt đầu với now = 0
            int last = 0;
            vector<int> candidates(S_all.begin(), S_all.end());
            shuffle(candidates.begin(), candidates.end(), gen); // Xáo trộn danh sách khách hàng

            unordered_set<int> toRemove;
            for (int customer : candidates)
            {
                if (checkInsertNext(route, customer, 0, now))
                {
                    route.push_back(customer);
                    last = customer;
                    
                    toRemove.insert(customer);
                }
                if(getRandomDouble(0,1)<0.2) break;

            }

        

            // Xóa khách đã được xếp
            for (int customer : toRemove)
            {
                S_all.erase(customer);
                S_role1.erase(customer); // Nếu khách có role = 1 thì cũng xóa khỏi S_role1
            }

            if (route.size() > 1) // Đảm bảo có khách trước khi lưu
            {
                route.push_back(0); // Quay về kho
                Route.push_back(route);
                Role.push_back(0); // Truck có role = 0
            }

            // cout << "role 1 còn:";
            // for(int x : S_role1)
            //     cout << x << " ";
        }

        // **3️⃣ Đảm bảo Truck đã phục vụ hết role = 1**
        if (!S_role1.empty())
        {
            cerr << "Lỗi: Truck chưa phục vụ hết khách role = 1!" << endl;
            return;
        }
        else{
            // cout << "đã hết role 1";
        }



        // **4️⃣ Drone phục vụ phần còn lại**
        int role = 1;
        double lastDroneTime = 0; // Thời điểm bắt đầu trip cuối cùng mà Drone thực hiện
        int lastDroneRole = -1;   // Role của Drone trước đó (-1 nghĩa là chưa có Drone nào)

        while (!S_all.empty())
        {
            bool can_continue = true;
            while (can_continue)
            {
                vector<int> route = {0};                                  // Bắt đầu từ kho
                double now = (lastDroneRole == role) ? lastDroneTime : 0; // Giữ nguyên now nếu cùng Drone, ngược lại reset về 0
                int last = 0;
                unordered_set<int> toRemove;

                vector<int> candidates(S_all.begin(), S_all.end());
                shuffle(candidates.begin(), candidates.end(), gen); // Xáo trộn khách còn lại

                for (int customer : candidates)
                {
                    if (checkInsertNext(route, customer, 0, now))
                    {
                        route.push_back(customer);
                        last = customer;
                        toRemove.insert(customer);
                    }
                    if (getRandomDouble(0, 1) < 0.2)
                        break;
                }


                for (int customer : toRemove)
                {
                    // cout << customer;
                    S_all.erase(customer);
                }

                if (route.size() > 1) // Chỉ lưu nếu có khách
                {
                    route.push_back(0); // Quay về kho
                    Route.push_back(route);
                    Role.push_back(role);
                    lastDroneTime = getTimeDroneTrip(route,now); // Cập nhật thời gian bắt đầu của trip cuối cùng của Drone
                    // cout <<"last time" << lastDroneTime;
                    lastDroneRole = role; // Ghi nhớ role của trip vừa thực hiện
                }
                else
                {
                    can_continue = false; // Không thể xếp thêm khách, tạo drone mới
                }
            }
            role++; // Tạo drone mới nếu cần
        }

        // **5️⃣ Lưu nghiệm hợp lệ**
        Solution sol(Route, Role);
        if (inserted.find(sol) == inserted.end())
            inserted.insert(sol);
    }
    // cout << endl << inserted.size();

    for (auto Sol : inserted)
        Mem.push_back(Sol);

    for (auto x: Mem){
        x.print();
    }
}

void Population::sort_by_domination_crowdingdistance(){
    vector<vector<int>> F(1);
    vector<int> n(Mem.size(), 0);
    vector<vector<int>> S(Mem.size());
    unordered_map<Solution,double> crowding_distance;

    // Xác định tập Pareto front
    for (int i = 0; i < Mem.size(); i++)
    {
        for (int j = 0; j < Mem.size(); j++)
        {
            if (Mem[i] < Mem[j]) // i thống trị j
            {
                S[i].push_back(j);
            }
            else if (Mem[j] < Mem[i]) // j thống trị i
            {
                n[i]++;
            }
        }
        if (n[i] == 0) // Nếu không ai thống trị i, thêm vào F[0]
        {
            F[0].push_back(i);
        }
    }

    int i = 0;
    while (i < F.size() && !F[i].empty())
    {
        vector<int> H;
        for (int x : F[i])
        {
            for (int y : S[x])
            {
                n[y]--;
                if (n[y] == 0)
                {
                    H.push_back(y);
                }
            }
        }
        if (!H.empty())
            F.push_back(H);
        i++;
    }
    if (F.back().empty())
        F.pop_back();

    unordered_map<Solution, int> rank;
    for (int i = 0; i < F.size(); i++)
    {
        for (int x : F[i])
        {
            rank[Mem[x]] = i;
        }
    }

    for (auto &front : F)
    {
        if (front.size() < 2)
            continue; // Nếu có 1 phần tử, không tính crowding distance

        // Khởi tạo khoảng cách chen chúc
        for (int x : front)
            crowding_distance[Mem[x]] = 0;

        // Tính crowding distance trên từng mục tiêu f1, f2, f3
        auto compute_crowding_distance = [&](auto cmp, auto accessor)
        {
            sort(front.begin(), front.end(), [&](int a, int b)
                 { return accessor(Mem[a]) < accessor(Mem[b]); });

            crowding_distance[Mem[front[0]]] = numeric_limits<double>::infinity();
            crowding_distance[Mem[front.back()]] = numeric_limits<double>::infinity();

            double minVal = accessor(Mem[front[0]]);
            double maxVal = accessor(Mem[front.back()]);

            if (maxVal == minVal) 
                return;

            for (int i = 1; i < front.size() - 1; i++)
            {
                crowding_distance[Mem[front[i]]] += (accessor(Mem[front[i + 1]]) - accessor(Mem[front[i - 1]])) / (maxVal - minVal);
            }
        };

        compute_crowding_distance([](const Solution &a, const Solution &b)
                                  { return a.f1 < b.f1; },
                                  [](const Solution &r)
                                  { return r.f1; });

        compute_crowding_distance([](const Solution &a, const Solution &b)
                                  { return a.f2 < b.f2; },
                                  [](const Solution &r)
                                  { return r.f2; });
    }

    // Sắp xếp `Mem` theo rank trước, crowding distance sau
    sort(Mem.begin(), Mem.end(), [&](const Solution &a, const Solution &b)
         {
             if (rank[a] != rank[b])
                 return rank[a] < rank[b];                       // Ưu tiên rank thấp hơn
             return crowding_distance[a] > crowding_distance[b]; // Nếu cùng rank, ưu tiên crowding distance lớn hơn
         });
}

Solution Population::TNselection(int k){
    vector<Solution> selected;
    while (selected.size() < k)
    {
        int index = getRandomNumber(0, Mem.size() - 1);
        selected.push_back(Mem[index]);
    }
    sort(selected.begin(), selected.end(), [&](Solution a, Solution b)
         { return a < b; });
    return selected[0];
}

Solution Population::crossover(Solution parent1, Solution parent2){
    // cout << " Bắt đầu lai" << endl;
    vector<vector<int>> childRoute;
    vector<int> childRole;

    unordered_set<int> S_all;
    unordered_set<int> S_role1;

    for (int i = 1; i < Cus.size();i++){
        S_all.insert(i);
        if (Cus[i].role == 1)
            S_role1.insert(i);
    }

    unordered_set<int> Route_truck_parent1;
    unordered_set<int> Route_truck_parent2;

    for (int i = 0; i < parent1.Role.size();i++){
        if (parent1.Role[i]!=0)
            break;
        Route_truck_parent1.insert(i);       
    }
    for (int i = 0; i < parent2.Role.size(); i++)
    {
        if (parent2.Role[i] != 0)
            break;
        Route_truck_parent2.insert(i);
    }

    int lastRole = 0;
    vector<unordered_set<int>> Set_Trip_Role_parent1(1);
    vector<unordered_set<int>> Set_Trip_Role_parent2(1);

    int index = 0;

    while (index < parent1.Role.size())
    {
        if (parent1.Role[index] == lastRole)
        {
            Set_Trip_Role_parent1[lastRole].insert(index);
        }
        else
        {
            Set_Trip_Role_parent1.resize(Set_Trip_Role_parent1.size() + 1);
            lastRole = parent1.Role[index];
            Set_Trip_Role_parent1[lastRole].insert(index);
        }
        index++;
    }

    lastRole = 0;
    index = 0;

    while (index < parent2.Role.size())
    {
        if (parent2.Role[index] == lastRole)
        {
            Set_Trip_Role_parent2[lastRole].insert(index);
        }
        else
        {
            Set_Trip_Role_parent2.resize(Set_Trip_Role_parent2.size() + 1);
            lastRole = parent2.Role[index];
            Set_Trip_Role_parent2[lastRole].insert(index);
        }
        index++;
    }

    // cout << "Khởi tạo xong " << endl;

    int k = Route_truck_parent1.size()>1? getRandomNumber(1, Route_truck_parent1.size() - 1):1;
    unordered_set<int> chosen;
    while(chosen.size() < k){
        //cout << chosen.size() << " " << k << endl;
        int in = getRandomNumber(0, Route_truck_parent1.size()-1);
        if (chosen.find(in) == chosen.end())
        {
            chosen.insert(in);
            //cout << in << endl;
            childRoute.push_back(parent1.Route[in]);
            for (int v : parent1.Route[in])
            {
                S_all.erase(v);
                S_role1.erase(v);
            }
            //cout << endl;
            childRole.push_back(0);
            // cout << "thêm ";
            // for(int j : childRoute.back()){
            //     cout << j << " ";
            // }
            // cout << childRole.back() << endl;
        }
    }
    // cout << "Thêm truck từ cha xong" <<endl;
    lastRole = 1;
    if (Set_Trip_Role_parent1.size() > 1)
    {
        int h = getRandomNumber(1, Set_Trip_Role_parent1.size() - 1);
        unordered_set<int> ch;
        while (ch.size() < h)
        {
            int id = getRandomNumber(1, Set_Trip_Role_parent1.size() - 1);
            if (ch.find(id) == ch.end())
            {
                ch.insert(id);
                for (int r : Set_Trip_Role_parent1[id])
                {
                    childRoute.push_back(parent1.Route[r]);
                    childRole.push_back(lastRole);

                    for (int R : parent1.Route[r])
                    {
                        S_all.erase(R);
                    }
                    // cout << "thêm ";
                    // for (int j : childRoute.back())
                    // {
                    //     cout << j << " ";
                    // }
                    // cout << childRole.back() << endl;
                }
                lastRole++;
            }
        }
    }
    // cout << "thêm drone từ cha xong" << endl;
    for(int id: Route_truck_parent2){
        bool flag = true;
        //cout << "S đang có: ";
        for(int x : S_all){
            //cout << x << " ";
        //cout << endl;
        for(int v : parent2.Route[id]){
            if(S_all.find(v)==S_all.end() && v!=0){
                flag = false;
            }
        }
    }
        //cout << flag << endl;
        if(flag){
            childRoute.push_back(parent2.Route[id]);
            childRole.push_back(0);
            for(int v : parent2.Route[id]){
                S_all.erase(v);
                S_role1.erase(v);
            }
            // cout << "thêm ";
            // for (int j : childRoute.back())
            // {
            //     cout << j << " ";
            // }
            // cout << childRole.back() << endl;
        }
    }
    // cout << " thêm truck từ mẹ xong"<<endl;

    
    if(Set_Trip_Role_parent2.size()>1){
    for (int index_set = 1; index_set < Set_Trip_Role_parent2.size();index_set++){
        bool flag = true;
        for(int r : Set_Trip_Role_parent2[index_set]){
            for(int cus : parent2.Route[r]){
                if(S_all.find(cus) == S_all.end()){
                    flag = false;
                    break;
                }
            }
        }
        if (flag){
            for (int r : Set_Trip_Role_parent2[index_set])
            {
                childRoute.push_back(parent2.Route[r]);
                for (int cus : parent2.Route[r])
                {
                    S_all.erase(cus);
                }
                childRole.push_back(lastRole);
            }
            // cout << "thêm ";
            // for (int j : childRoute.back())
            // {
            //     cout << j << " ";
            // }
            // cout << childRole.back() << endl;
            // lastRole++;
        }
    }
    }
    // cout << "thêm drone từ mẹ xong" << endl;
    int index_child_trip = 0;
    for (; index_child_trip < childRoute.size(); index_child_trip++)
    {
        if(childRole[index_child_trip]!=0)
            break;
        double now = 0;
        vector<int> trip = childRoute[index_child_trip];
        vector<int> todelete;
        trip.pop_back();
        for(int x : S_all){
            if(checkInsertNext(trip,x,childRole[index_child_trip],now)){
                trip.push_back(x);
                todelete.push_back(x);
            }
        }
        trip.push_back(0);
        childRoute[index_child_trip] = trip;
        // cout << "thêm ";
        // for (int j : childRoute[index_child_trip])
        // {
        //     cout << j << " ";
        // }
        // cout << childRole[index_child_trip] << endl;
        for(int y : todelete){
            S_all.erase(y);
            S_role1.erase(y);
        }
    }
    //cout << "chèn xong truck con " << endl;
    lastRole = 0;
    int now = 0;
    
    for (; index_child_trip < childRoute.size(); index_child_trip++)
    {
        //cout << index_child_trip <<" "<< childRoute.size() << endl;
        if(childRole[index_child_trip]!=lastRole){
            lastRole = childRole[index_child_trip];
            now = 0;
        }
        //cout << " bây giờ " << now << endl;
        vector<int> trip = childRoute[index_child_trip];
        vector<int> todelete;
        trip.pop_back();
        for (int x : S_all)
        {
            if (checkInsertNext(trip, x, childRole[index_child_trip], now))
            {
                trip.push_back(x);
                todelete.push_back(x);
            }
        }
        trip.push_back(0);
        childRoute[index_child_trip] = trip;
        // cout << "thêm ";
        // for (int j : childRoute[index_child_trip])
        // {
        //     cout << j << " ";
        // }
        // cout << childRole[index_child_trip] << endl;
        now = getTimeDroneTrip(trip,now);
        for (int y : todelete)
        {
            S_all.erase(y);
            S_role1.erase(y);
        }
    }
    //cout << "chèn xong drone con" << endl;

    now = 0;
    lastRole++;
    //cout << num_Custom - S_all.size() << min_custom_serve << endl;
    while(num_Custom-S_all.size() < min_custom_serve){
        int choose = getRandomNumber(0, 1);
        //cout << choose << endl;
        if(choose == 0){
            //cout << "thêm truck" << endl;
            vector<int> truck_trip;
            truck_trip.push_back(0);
            vector<int> todelete;
            for(int v : S_all){
                if(checkInsertNext(truck_trip,v,0,0)){
                    truck_trip.push_back(v);
                    todelete.push_back(v);
                }
            }
            truck_trip.push_back(0);
            childRoute.push_back(truck_trip);
            childRole.push_back(0);
            for(int v: todelete){
                S_all.erase(v);
                S_role1.erase(v);
            }
        }
        else{
            //cout << " bây giờ " << now << endl;
            bool can_insert = false;
            vector<int> drone_trip;
            vector<int> todelete;
            drone_trip.push_back(0);
            for (int v : S_all)
            {
                if (checkInsertNext(drone_trip, v, lastRole, now))
                {
                    can_insert = true;
                    break;
                }
            }
            if(can_insert)
            {
                //cout << can_insert << endl;
                for (int v : S_all)
                {   
                    if (checkInsertNext(drone_trip, v, lastRole, now))
                    {
                        drone_trip.push_back(v);
                        todelete.push_back(v);
                    }
                    
                }
                drone_trip.push_back(0);
                childRoute.push_back(drone_trip);
                childRole.push_back(lastRole);
                // cout << "thêm ";
                // for (int j : childRoute.back())
                // {
                //     cout << j << " ";
                // }
                // cout << childRole.back() << endl;

                now = getTimeDroneTrip(drone_trip, now);
                for (int v : todelete)
                {
                    S_all.erase(v);
                    S_role1.erase(v);
                }
            }
            else{
                lastRole++;
                now = 0;                
            }
        }
    }

    return Solution(childRoute, childRole);
}

Solution Population::mutate(Solution parents, double mutation_rate){
    if (getRandomDouble(0, 1) >= mutation_rate)
        return parents;
    // cout << " bị mutate " << endl;
    int lastRole = 0;
    bool mutate = false;
    vector<unordered_set<int>> Set_Trip_Role_parent(1);
    auto childRoute = parents.Route;
    auto childRole = parents.Role;

    int index = 0;

    while (index < parents.Role.size())
    {
        
        if (parents.Role[index] == lastRole)
        {
            Set_Trip_Role_parent[lastRole].insert(index);
        }
        else
        {
            Set_Trip_Role_parent.resize(Set_Trip_Role_parent.size() + 1);
            lastRole = parents.Role[index];
            Set_Trip_Role_parent[lastRole].insert(index);
        }
        index++;
    }

    while(mutate == false){
        
        int choose = getRandomNumber(0, 4);
        //cout << "choose: " << choose << endl;
        if (choose == 0)
        { // đảo
            // cout << "mutate đảo" << endl;
            int route_reverse = getRandomNumber(0, childRoute.size() - 1);
            if (childRoute[route_reverse].size() > 3)
            {
                int l = getRandomNumber(1, childRoute[route_reverse].size() - 3);
                int r = getRandomNumber(l + 1, childRoute[route_reverse].size() - 2);
                mutate = true;
                reverse(childRoute[route_reverse].begin() + l, childRoute[route_reverse].begin() + r + 1);
            }
        
    }
    else if(choose == 1){//bớt chỗ này thêm chỗ khác
        // cout << "mutate đổi" << endl;
        if(Set_Trip_Role_parent.size()==1) continue;
        int role_choose1 = getRandomNumber(0, Set_Trip_Role_parent.size()-1);
        int role_choose2 = getRandomNumber(0, Set_Trip_Role_parent.size() - 1);
        while(role_choose2 == role_choose1 && role_choose1!=0){
            role_choose2 = getRandomNumber(0, Set_Trip_Role_parent.size() - 1);
        }
        int id1 = getRandomNumber(0, Set_Trip_Role_parent[role_choose1].size() - 1);
        auto it1 = next(Set_Trip_Role_parent[role_choose1].begin(), id1);
        id1 = *it1;
        int id2 = getRandomNumber(0, Set_Trip_Role_parent[role_choose2].size() - 1);
        auto it2 = next(Set_Trip_Role_parent[role_choose2].begin(), id2);
        id2 = *it2;

        if(childRoute[id2].size() < 4) continue;

        int v = getRandomNumber(1, childRoute[id2].size() - 2);
        int insertPos = getRandomNumber(1, childRoute[id1].size() - 1);
        childRoute[id1].insert(childRoute[id1].begin() + insertPos, childRoute[id2][v]);
        childRoute[id2].erase(childRoute[id2].begin() + v);
        mutate = true;
    }

    else if(choose == 2){// thay role
        // cout << "mutate thay role" << endl;
        int role_choose1 = getRandomNumber(0, Set_Trip_Role_parent.size() - 1);
        int role_choose2 = getRandomNumber(0, Set_Trip_Role_parent.size() - 1);
        //cout << role_choose1 << " " << role_choose2 << endl;
        while (role_choose2 == role_choose1 && role_choose1 != 0)
        {
            role_choose2 = getRandomNumber(0, Set_Trip_Role_parent.size() - 1);
        }
        int id1 = getRandomNumber(0, Set_Trip_Role_parent[role_choose1].size() - 1);
        auto it1 = next(Set_Trip_Role_parent[role_choose1].begin(), id1);
        id1 = *it1;
        //cout << id1 << " "  << endl;
        //cout << childRole[id1] << endl;

        childRole[id1] = role_choose2;
        //cout << childRole[id1] << endl;

        //cout << "sau mutate" << endl;
        // for (int b = 0; b < childRoute.size();b++){
        //     for (int c = 0; c < childRoute[b].size();c++){
        //         cout << childRoute[b][c] << " ";
        //     }
        //     cout << childRole[b] << endl;
        // }

            mutate = true;
    }
    else if(choose == 3){// nối cùng role
        // cout << "mutate nối cùng" << endl;
        // if (Set_Trip_Role_parent.size() < 2) continue;
        int role_choose1 = getRandomNumber(0, Set_Trip_Role_parent.size() - 1);
        // cout << "role: " << role_choose1 << endl;
        if (Set_Trip_Role_parent[role_choose1].size()<2) continue;
            int id1 = getRandomNumber(0, Set_Trip_Role_parent[role_choose1].size() - 1);
        int id2 = getRandomNumber(0, Set_Trip_Role_parent[role_choose1].size() - 1);
        // cout << "id1 " << id1 << " id2 " << id2 << endl;
        while(id2 == id1){

            id2 = getRandomNumber(0, Set_Trip_Role_parent[role_choose1].size() - 1);
        }
        auto it1 = next(Set_Trip_Role_parent[role_choose1].begin(), id1);
        id1 = *it1;
        auto it2 = next(Set_Trip_Role_parent[role_choose1].begin(), id2);
        id2 = *it2;

        // cout << "id1 "<<id1 << " id2 " <<id2<< endl;

        vector<int> temp;
        temp.push_back(0);
        for (int i = 1; i < childRoute[id1].size()-1;++i){
            temp.push_back(childRoute[id1][i]);
        }
        for (int i = 1; i < childRoute[id2].size() - 1; ++i)
        {
            temp.push_back(childRoute[id2][i]);
        }
        temp.push_back(0);

        // cout << " sau nối: ";
        // for(int x: temp)
        //     cout << x << " ";
        // cout << endl;

        if (id1 > id2)
        {
            swap(id1, id2); // Đảm bảo xóa theo thứ tự đúng
        }
        childRoute.erase(childRoute.begin() + id2);
        childRole.erase(childRole.begin() + id2);
        childRoute.erase(childRoute.begin() + id1);
        childRole.erase(childRole.begin() + id1);

        childRoute.push_back(temp);
        childRole.push_back(role_choose1);

        mutate = true;
    }

    else
    { // nối khác role
        // cout << "mutate nối khác" << endl;
        if (Set_Trip_Role_parent.size() < 2)
            continue;
        int role_choose1 = getRandomNumber(0, Set_Trip_Role_parent.size() - 1);
        int role_choose2 = getRandomNumber(0, Set_Trip_Role_parent.size() - 1);
        while (role_choose2 == role_choose1 && role_choose1 != 0)
        {
            role_choose2 = getRandomNumber(0, Set_Trip_Role_parent.size() - 1);
        }
        int id1 = getRandomNumber(0, Set_Trip_Role_parent[role_choose1].size() - 1);
        int id2 = getRandomNumber(0, Set_Trip_Role_parent[role_choose2].size() - 1);
        auto it1 = next(Set_Trip_Role_parent[role_choose1].begin(), id1);
        id1 = *it1;
        auto it2 = next(Set_Trip_Role_parent[role_choose2].begin(), id2);
        id2 = *it2;

        vector<int> temp;
        temp.push_back(0);
        for (int i = 1; i < childRoute[id1].size() - 1; ++i)
        {
            temp.push_back(childRoute[id1][i]);
        }
        for (int i = 1; i < childRoute[id2].size() - 1; ++i)
        {
            temp.push_back(childRoute[id2][i]);
        }
        temp.push_back(0);
        if (Set_Trip_Role_parent[role_choose2].size() == 1)
        {
            int index_big_role = *Set_Trip_Role_parent[role_choose2].begin();
            for (int _z = index_big_role + 1; _z < childRole.size(); _z++){
                childRole[_z]--;
            }
        }

        if (id1 > id2)
        {
            swap(id1, id2); // Đảm bảo xóa theo thứ tự đúng
        }
        childRoute.erase(childRoute.begin() + id2);
        childRole.erase(childRole.begin() + id2);
        childRoute.erase(childRoute.begin() + id1);
        childRole.erase(childRole.begin() + id1);

        childRoute.push_back(temp);
        childRole.push_back(role_choose1<role_choose2? role_choose1: role_choose2);
        mutate = true;
    }
    //cout << " đang mutate :" << mutate << endl;
    }

    //cout << "sau mutate" << endl;
    // for (int b = 0; b < childRoute.size(); b++)
    // {
    //     for (int c = 0; c < childRoute[b].size(); c++)
    //     {
    //         cout << childRoute[b][c] << " ";
    //     }
    //     cout << childRole[b] << endl;
    // }

    mutate = true;
    return Solution(childRoute, childRole);
}
void Population::create_next_member(){
    vector<Solution> next_member;
    for (int i = 0; i < size; i++)
    {
        next_member.push_back(Mem[i]);
    }
    Mem = next_member;

}

Solution Population::SelectByParent1(Solution parent1){
    double jaccard =1;
    int last = -1;
    for (int i = 0; i < Mem.size();i++){
        if(jaccardSimilarity(parent1.Route,Mem[i].Route)<=jaccard){
            jaccard = jaccardSimilarity(parent1.Route, Mem[i].Route);
            last = i;
        }
    }
    return Mem[last];
}
void Population::Genetic(string Method, double mutation_rate, int stagnation){
    cout << " Start genetic" << endl;
    if (Method == "NSGA_II")
    {
        NSGA_II<Population, Solution> nsga_ii(*this);

        nsga_ii.NSGA_II_Genetic(mutation_rate, stagnation);
    }
    else if(Method == "MOEA"){
        MOEA<Population, Solution> moea(*this);
        moea.MOEA_Genetic(mutation_rate, stagnation);
    }

}