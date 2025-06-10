#include "function.h"
#include "Population.h"
#include "NSGA.h"
#include "MOEA.h"
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <algorithm> // Để xáo trộn ngẫu nhiên

Population::Population(int _n, int _size, bool _opt) : n(_n), size(_size),opt(_opt)
{
    unordered_set<Solution> inserted;
    int n_random_shuffle = size;
    cout << "Tạo cá thể bởi RD: " << n_random_shuffle << endl;
    // cout << "n_random_shuffle = " << n_random_shuffle << endl;
    // cout << "inserted.size() = " << inserted.size() << endl;

    while (inserted.size() < n_random_shuffle)
    {
        cout << "inserted.size() = " << inserted.size() << endl;
        int num_truck = 0;
        int num_drone = 0;
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
        cout << "ok1" << endl;

        // **2️⃣ Truck phục vụ ngẫu nhiên khách hàng trước**
        while (!S_role1.empty() && num_truck<max_truck ) // Lặp đến khi không còn khách role = 1
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
                if (getRandomDouble(0, 1) < drop_insert_custom)
                    break;
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
                num_truck++;
            }

            // cout << "role 1 còn:";
            // for(int x : S_role1)
            //     cout << x << " ";

            if(getRandomDouble(0,1)<drop_insert_trip) break;
        }
        cout << "ok2" << endl;

        // **3️⃣ Đảm bảo Truck đã phục vụ hết role = 1**
        for(int Cus_role1 : S_role1){
            S_all.erase(Cus_role1);
        }



        // **4️⃣ Drone phục vụ phần còn lại**
        int role = 1;
        double lastDroneTime = 0; // Thời điểm bắt đầu trip cuối cùng mà Drone thực hiện
        int lastDroneRole = -1;   // Role của Drone trước đó (-1 nghĩa là chưa có Drone nào)

        while (!S_all.empty() && num_drone<max_drone)
        {
            bool can_continue = true;
            while (can_continue)
            {
                vector<int> route = {0};                                  // Bắt đầu từ kho
                double now = (lastDroneRole == role) ? lastDroneTime : 0; // Giữ nguyên now nếu cùng Drone, ngược lại reset về 0
                lastDroneRole = role; // Cập nhật role của Drone
                int last = 0;
                unordered_set<int> toRemove;

                vector<int> candidates(S_all.begin(), S_all.end());
                shuffle(candidates.begin(), candidates.end(), gen); // Xáo trộn khách còn lại

                for (int customer : candidates)
                {
                    if (checkInsertNext(route, customer, role, now))
                     {   
                        //cout << " now = " << now << " ";
                    //     cout << int(checkInsertNext(route, customer, role, now)) << endl;
                        route.push_back(customer);
                        last = customer;
                        toRemove.insert(customer);
                        if (getRandomDouble(0, 1) < drop_insert_custom)
                            break;
                    }
                }


                for (int customer : toRemove)
                {
                    // cout << customer;
                    S_all.erase(customer);
                }

                if (route.size() > 1) // Chỉ lưu nếu có khách
                {
                    route.push_back(0); // Quay về kho
                   
                    // cout << " thêm route " << endl;
                    // for (int x : route)
                    //     cout << x << " ";
                    // cout << endl;
                    // cout << "lastDroneTime = " << lastDroneTime << endl;
                    Route.push_back(route);
                    Role.push_back(role);
                    lastDroneTime = getTimeDroneTrip(route,now); // Cập nhật thời gian bắt đầu của trip cuối cùng của Drone
                    
                    lastDroneRole = role; // Ghi nhớ role của trip vừa thực hiện
                    if(getRandomDouble(0,1)<drop_insert_trip){
                        can_continue = false; // Không thể xếp thêm khách, tạo drone mới
                        num_drone++;
                    }
                }
                else
                {
                    can_continue = false; // Không thể xếp thêm khách, tạo drone mới
                    num_drone++;
                }
            }
            role++; // Tạo drone mới nếu cần
        }
        cout << "ok3" << endl;

        // **5️⃣ Lưu nghiệm hợp lệ**
        Solution sol(Route, Role);
        sol.print();
        if (inserted.find(sol) == inserted.end())
            inserted.insert(sol);
    }
    // cout << endl << inserted.size();

    for (auto Sol : inserted)
        {   
            cout << "trước khi tối ưu hóa" << endl;
            Sol.print();
            if(opt) Sol.optimize_trip();
            cout << "sau khi tối ưu hóa" << endl;
            Sol.print();
            Mem.push_back(Sol);
        }
    // for (auto Sol : inserted)
    // {
       
    //     cout << "trước khi tối ưu hóa" << endl;
    //     Sol.print();
    //     cout << "sau khi tối ưu hóa" << endl;
    //     Sol.optimize_trip();
    //     Solution test(Sol.Route, Sol.Role);
    //     Sol.print();
    //     test.print();
    //     if (test.f1 != Sol.f1 || test.f2 != Sol.f2)
    //     {
    //         cout << "Lỗi nghiệm khởi tạo" << endl;
    //         cout << "f1 = " << test.f1 << " f2 = " << test.f2 << endl;
    //         cout << "f1 = " << Sol.f1 << " f2 = " << Sol.f2 << endl;
    //     }
    // }
}
void Population::basic_sort()
{
    vector<vector<int>> F(1); // Các tầng Pareto front
    vector<int> n(Mem.size(), 0);
    vector<vector<int>> S(Mem.size());

    // Xác định tập Pareto front
    for (int i = 0; i < Mem.size(); i++)
    {
        for (int j = 0; j < Mem.size(); j++)
        {
            if (Mem[i] < Mem[j])
            {
                S[i].push_back(j);
            }
            else if (Mem[j] < Mem[i])
            {
                n[i]++;
            }
        }
        if (n[i] == 0)
        {
            F[0].push_back(i);
        }
    }

    // Lưu lại Pareto front đầu tiên
    vector<pair<int, double>> P;
    for (int i : F[0])
    {
        P.push_back({Mem[i].f1, Mem[i].f2});
    }
    pareto_front_in_generation.push_back(P);

    // Xây dựng tiếp các front sau
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

    // Gán rank cho từng Solution
    unordered_map<Solution, int> rank;
    for (int i = 0; i < F.size(); i++)
    {
        for (int x : F[i])
        {
            rank[Mem[x]] = i;
        }
    }

    // Chỉ sắp xếp Mem theo rank
    sort(Mem.begin(), Mem.end(), [&](const Solution &a, const Solution &b)
         { return rank[a] < rank[b]; });
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
    vector<pair<int, double>> P;
    for (int i : F[0])
    {
        P.push_back({Mem[i].f1, Mem[i].f2});
    }
    pareto_front_in_generation.push_back(P);

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
    // cout << "Trước khi sắp xếp :" << endl;
    // for(auto u : Mem)
    //     u.print();
    sort(Mem.begin(), Mem.end(), [&](const Solution &a, const Solution &b)
         {
             if (rank[a] != rank[b])
                 return rank[a] < rank[b];                       // Ưu tiên rank thấp hơn
             return crowding_distance[a] > crowding_distance[b]; // Nếu cùng rank, ưu tiên crowding distance lớn hơn
         });
    // cout << "sau khi sắp xếp :" << endl;
    // for (auto u : Mem)
    //     u.print();
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
    // cout << "cha: " << endl;
    // parent1.print();
    // cout << "mẹ: " << endl;
    // parent2.print();
    vector<vector<int>> childRoute;
    vector<int> childRole;
    
    int num_truck = 0;
    int num_drone = 0;

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

    //cout << "ok3" << endl;

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

    //cout << "Khởi tạo xong " << endl;

    int k = Route_truck_parent1.size()>1? getRandomNumber(1, Route_truck_parent1.size() - 1):1;
    if(Route_truck_parent1.size()==0){
        k = 0;
    }
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
            num_truck++;
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

        unordered_set<int> selected_roles;
        while (selected_roles.size() < h)
        {
            int id = getRandomNumber(1, Set_Trip_Role_parent1.size() - 1);
            selected_roles.insert(id);
        }

        for (int choose_role : selected_roles)
        {
            for (int ide = 0; ide < parent1.Route.size(); ide++)
            {
                if (parent1.Role[ide] == choose_role)
                {
                    childRoute.push_back(parent1.Route[ide]);
                    for (int v : parent1.Route[ide])
                    {
                        S_all.erase(v);
                        S_role1.erase(v);
                    }
                    childRole.push_back(lastRole);
                    // cout << "thêm ";
                    // for (int j : childRoute.back())
                    // {
                    //     cout << j << " ";
                    // // }
                    // cout << "thêm route" << endl;
                    // for (int j : childRoute.back())
                    // {
                    //     cout << j << " ";
                    // }
                    // cout << childRole.back() << endl;
                }
                
                
            }
            
            
            lastRole++;
            num_drone++;
        }
    }
    //  cout << "thêm drone từ cha xong" << endl;
    // Solution(childRoute, childRole).print();
    // cout << "thêm drone từ cha xong" << endl;
    // Solution(childRoute, childRole).print();
    for(int id: Route_truck_parent2){
        if(num_truck>=max_truck)
            break;
        bool flag = true;
        // cout << "S đang có: ";
        // for(int x : S_all){
        //     cout << x << " ";
        // cout << endl;
        // }
        for(int v : parent2.Route[id]){
            if(S_all.find(v)==S_all.end() && v!=0){
                flag = false;
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
            num_truck++;
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
        if(num_drone>=max_drone)
            break;
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
            for (int ide = 0; ide < parent2.Route.size(); ide++){
                if (parent2.Role[ide] == index_set)
                {
                    childRoute.push_back(parent2.Route[ide]);
                    for (int v : parent2.Route[ide])
                    {
                        S_all.erase(v);
                        S_role1.erase(v);
                    }
                    childRole.push_back(lastRole);
                }
            }
                // cout << "thêm ";
                // for (int j : childRoute.back())
                // {
                //     cout << j << " ";
                // }
                // cout << childRole.back() << endl;
                num_drone++;
                lastRole++;
        }
    }
    }
    // cout << "thêm drone từ mẹ xong" << endl;
    // Solution(childRoute, childRole).print();

    //sắp xếp lại childRoute và childRole theo thứ tự tăng dần của role


    vector<pair<int,vector<int>>> childRouteRole;
    for (int i = 0; i < childRoute.size(); i++)
    {
        childRouteRole.push_back({childRole[i], childRoute[i]});
    }

    // Dùng stable_sort để giữ nguyên thứ tự của các phần tử cùng role
    stable_sort(childRouteRole.begin(), childRouteRole.end(),
                [](const pair<int, vector<int>> &a, const pair<int, vector<int>> &b)
                {
                    return a.first < b.first;
                });

    // Tách lại ra
    for (int i = 0; i < childRouteRole.size(); i++)
    {
        childRoute[i] = childRouteRole[i].second;
        childRole[i] = childRouteRole[i].first;
    }

    

    // cout << "sắp xếp lại childRoute và childRole xong" << endl;
    // cout << "childRoute: " << endl;
    


    vector<int> childMark(childRoute.size(),1);
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
                
                
                // cout << " lỗi: " << int(checkInsertNext(trip,x,childRole[index_child_trip],0)) << endl;
                trip.push_back(x);
                todelete.push_back(x);
                childMark[index_child_trip] = 0;
                // cout << " trip hiện tại có" << endl;
                // for (int j : trip)
                // {
                //     cout << j << " ";
                // }
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
    // cout << "chèn xong truck " << endl;
    
    // cout << "chèn xong truck con " << endl;
    // Solution(childRoute, childRole).print();
    lastRole = 0;
    double now = 0;
    
    for (; index_child_trip < childRoute.size(); index_child_trip++)
    {
        // cout << index_child_trip <<" "<< childRoute.size() << endl;
        if(childRole[index_child_trip]!=lastRole){
            lastRole = childRole[index_child_trip];
            now = 0;
        }
        // cout << " bây giờ " << now << endl;
        vector<int> trip = childRoute[index_child_trip];
        vector<int> todelete;
        trip.pop_back();
        for (int x : S_all)
        {
            if (checkInsertNext(trip, x, childRole[index_child_trip], now))
            {
                // cout << "đang test" << endl;
                bool flag = true;
                double now_virtual = now;
                auto temp = trip;
                temp.push_back(x);
                temp.push_back(0);
                // cout << "temp hợp lệ: " << int(checkValidVector(temp, childRole[index_child_trip], now_virtual)) << endl;
                now_virtual = getTimeDroneTrip(temp, now_virtual);

                
                for(int ide = index_child_trip+1; ide < childRoute.size(); ide++){
                    if(childRole[ide] == childRole[index_child_trip]){
                        if(!checkValidVector(childRoute[ide], childRole[ide], now_virtual)){
                            flag = false;
                            break;
                        }
                        now_virtual = getTimeDroneTrip(childRoute[ide], now_virtual);
                    }
                    else{
                        break;
                    }
                }
                // cout << "flag = " << flag << endl;
                if(!flag){
                    continue;
                }
                trip.push_back(x);
                todelete.push_back(x);
                childMark[index_child_trip] = 0;
            }
        }
        trip.push_back(0);
        // cout << "trip hợp lệ :" << int(checkValidVector(trip, childRole[index_child_trip], now)) << endl;
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


    // cout << "chèn xong drone con" << endl;
    // Solution(childRoute, childRole).print();
    now = 0;
    lastRole++;
    //cout << num_Custom - S_all.size() << min_custom_serve << endl;
    while((num_drone<max_drone||num_truck<max_truck)&&!S_all.empty()){
        if(getRandomDouble(0,1)<drop_insert_trip) break;
        int choose = getRandomNumber(0, 1);
        //cout << choose << endl;
        if (choose == 0 && num_truck < max_truck)
        {
            //cout << "thêm truck" << endl;
            vector<int> truck_trip;
            truck_trip.push_back(0);
            vector<int> todelete;
            for(int v : S_all){
                if(checkInsertNext(truck_trip,v,0,0)){
                    truck_trip.push_back(v);
                    todelete.push_back(v);
                    if(getRandomDouble(0,1)<drop_insert_custom)
                        break;
                }
            }
            truck_trip.push_back(0);
            childRoute.push_back(truck_trip);
            childRole.push_back(0);
            num_truck++;
            for(int v: todelete){
                S_all.erase(v);
                S_role1.erase(v);
            }
            childMark.push_back(0);
        }
        else{

            if(num_drone<max_drone){
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
                        if(getRandomDouble(0,1)<drop_insert_custom)
                            break;
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
                num_drone++;
                lastRole++;
                now = 0;                
            }
        }
        childMark.push_back(1);
        }
        
    }
    // cout << "lai ghép xong" << endl;
   
    childRouteRole.clear();
    for (int i = 0; i < childRoute.size(); i++)
    {
        childRouteRole.push_back({childRole[i], childRoute[i]});
    }

    // Dùng stable_sort để giữ nguyên thứ tự của các phần tử cùng role
    stable_sort(childRouteRole.begin(), childRouteRole.end(),
                [](const pair<int, vector<int>> &a, const pair<int, vector<int>> &b)
                {
                    return a.first < b.first;
                });

    // Tách lại ra
    for (int i = 0; i < childRouteRole.size(); i++)
    {
        childRoute[i] = childRouteRole[i].second;
        childRole[i] = childRouteRole[i].first;
    }
    // cout << "childRoute: " << endl;
    // for (int i = 0; i < childRoute.size(); i++)
    // {
    //     cout << "role: " << childRole[i] << " ";
    //     for (int j : childRoute[i])
    //     {
    //         cout << j << " ";
    //     }
    //     cout << endl;
    // }
    Solution child(childRoute, childRole, childMark);
    // cout << "truớc khi tối ưu hóa" << endl;
    // child.print();
    if(opt) child.optimize_trip();
    // cout << "sau khi tối ưu hóa" << endl;
    // Solution(child.Route, child.Role).print();
    return child;
}

Solution Population::mutate(Solution parents, double mutation_rate){
    if (getRandomDouble(0, 1) >= mutation_rate)
        return parents;
    // cout << " bị mutate " << endl;
    // parents.print();
    // int lastRole = 0;
    // bool mutate = false;
    // vector<unordered_set<int>> Set_Trip_Role_parent(1);
    auto childRoute = parents.Route;
    auto childRole = parents.Role;
    auto childMark = parents.Mark;

    bool mutate = false;

    int id = getRandomNumber(0, childRole.size() - 1);

    while (childRoute[id].size() < 4) id = getRandomNumber(0, childRole.size() - 1);

    int erId = getRandomNumber(1, childRoute[id].size() - 2);

    childRoute[id].erase(childRoute[id].begin() + erId);
    childMark[id] = 0;
    Solution child(childRoute, childRole, childMark);
    // child.print();
    if(opt) child.optimize_trip();
    return child;

    // int index = 0;
    // int Tryindex = 0;

    // do{
    // while (index < parents.Role.size())
    // {
        
    //     if (parents.Role[index] == lastRole)
    //     {
    //         Set_Trip_Role_parent[lastRole].insert(index);
    //     }
    //     else
    //     {
    //         Set_Trip_Role_parent.resize(Set_Trip_Role_parent.size() + 1);
    //         lastRole = parents.Role[index];
    //         Set_Trip_Role_parent[lastRole].insert(index);
    //     }
    //     index++;
    // }

    // while(mutate == false){
        
    //     int choose = getRandomNumber(2, 2);
        //cout << "choose: " << choose << endl;
    //     if(choose == 1){//bớt chỗ này thêm chỗ khác
    //     cout << "mutate đổi" << endl;
    //     if(Set_Trip_Role_parent.size()==1) continue;
    //     int role_choose1 = getRandomNumber(0, Set_Trip_Role_parent.size()-1);
    //     int role_choose2 = getRandomNumber(0, Set_Trip_Role_parent.size() - 1);
    //     while(role_choose2 == role_choose1 && role_choose1!=0){
    //         role_choose2 = getRandomNumber(0, Set_Trip_Role_parent.size() - 1);
    //     }
    //     int id1 = getRandomNumber(0, Set_Trip_Role_parent[role_choose1].size() - 1);
    //     auto it1 = next(Set_Trip_Role_parent[role_choose1].begin(), id1);
    //     id1 = *it1;
    //     int id2 = getRandomNumber(0, Set_Trip_Role_parent[role_choose2].size() - 1);
    //     auto it2 = next(Set_Trip_Role_parent[role_choose2].begin(), id2);
    //     id2 = *it2;

    //     if(childRoute[id2].size() < 4) continue;

    //     int v = getRandomNumber(1, childRoute[id2].size() - 2);
    //     int insertPos = getRandomNumber(1, childRoute[id1].size() - 1);
    //     childRoute[id1].insert(childRoute[id1].begin() + insertPos, childRoute[id2][v]);
    //     childRoute[id2].erase(childRoute[id2].begin() + v);
    //     childMark[id1] = childMark[id2] = 0;
    //     mutate = true;
    // }

    // else
    //  if(choose == 2){// thay role
    //     cout << "mutate thay role" << endl;
    //     int role_choose1 = getRandomNumber(0, Set_Trip_Role_parent.size() - 1);
    //     int role_choose2 = getRandomNumber(0, Set_Trip_Role_parent.size() - 1);
    //     //cout << role_choose1 << " " << role_choose2 << endl;
    //     while (role_choose2 == role_choose1 && role_choose1 != 0)
    //     {
    //         role_choose2 = getRandomNumber(0, Set_Trip_Role_parent.size() - 1);
    //     }
    //     int id1 = getRandomNumber(0, Set_Trip_Role_parent[role_choose1].size() - 1);
    //     auto it1 = next(Set_Trip_Role_parent[role_choose1].begin(), id1);
    //     id1 = *it1;
    //     //cout << id1 << " "  << endl;
    //     //cout << childRole[id1] << endl;
    //     if (role_choose1 != 0 && role_choose2==0){
    //         if(parents.num_truck+1>max_truck) continue;
    //     }

    //         childRole[id1] = role_choose2;
    //         childMark[id1] = 0;
    //         // cout << childRole[id1] << endl;

    //         // cout << "sau mutate" << endl;
    //         //  for (int b = 0; b < childRoute.size();b++){
    //         //      for (int c = 0; c < childRoute[b].size();c++){
    //         //          cout << childRoute[b][c] << " ";
    //         //      }
    //         //      cout << childRole[b] << endl;
    //         //  }

    //         mutate = true;
    // }
    // else if(choose == 3){// nối cùng role
    //     cout << "mutate nối cùng" << endl;
    //     // if (Set_Trip_Role_parent.size() < 2) continue;
    //     int role_choose1 = getRandomNumber(0, Set_Trip_Role_parent.size() - 1);
    //     if (role_choose1!=0){
    //         for (int trip : Set_Trip_Role_parent[role_choose1])
    //         {
    //             childMark[trip] = 0;
    //         }
    //     }
    //         // cout << "role: " << role_choose1 << endl;
    //     if (Set_Trip_Role_parent[role_choose1].size() < 2)
    //             continue;
    //     int id1 = getRandomNumber(0, Set_Trip_Role_parent[role_choose1].size() - 1);
    //     int id2 = getRandomNumber(0, Set_Trip_Role_parent[role_choose1].size() - 1);
    //     // cout << "id1 " << id1 << " id2 " << id2 << endl;
    //     while(id2 == id1){

    //         id2 = getRandomNumber(0, Set_Trip_Role_parent[role_choose1].size() - 1);
    //     }
    //     auto it1 = next(Set_Trip_Role_parent[role_choose1].begin(), id1);
    //     id1 = *it1;
    //     auto it2 = next(Set_Trip_Role_parent[role_choose1].begin(), id2);
    //     id2 = *it2;

    //     // cout << "id1 "<<id1 << " id2 " <<id2<< endl;

    //     vector<int> temp;
    //     temp.push_back(0);
    //     for (int i = 1; i < childRoute[id1].size()-1;++i){
    //         temp.push_back(childRoute[id1][i]);
    //     }
    //     for (int i = 1; i < childRoute[id2].size() - 1; ++i)
    //     {
    //         temp.push_back(childRoute[id2][i]);
    //     }
    //     temp.push_back(0);

    //     // cout << " sau nối: ";
    //     // for(int x: temp)
    //     //     cout << x << " ";
    //     // cout << endl;

    //     if (id1 > id2)
    //     {
    //         swap(id1, id2); // Đảm bảo xóa theo thứ tự đúng
    //     }
    //     childRoute.erase(childRoute.begin() + id2);
    //     childRole.erase(childRole.begin() + id2);
    //     childMark.erase(childMark.begin() + id2);
    //     childRoute.erase(childRoute.begin() + id1);
    //     childRole.erase(childRole.begin() + id1);
    //     childMark.erase(childMark.begin() + id1);

    //     childRoute.push_back(temp);
    //     childRole.push_back(role_choose1);
    //     childMark.push_back(0);

    //     mutate = true;
    // }

    // else
    // { // nối khác role
    //     cout << "mutate nối khác" << endl;
    //     if (Set_Trip_Role_parent.size() < 2)
    //         continue;
    //     int role_choose1 = getRandomNumber(0, Set_Trip_Role_parent.size() - 1);
    //     int role_choose2 = getRandomNumber(0, Set_Trip_Role_parent.size() - 1);
    //     while (role_choose2 == role_choose1 && role_choose1 != 0)
    //     {
    //         role_choose2 = getRandomNumber(0, Set_Trip_Role_parent.size() - 1);
    //     }
    //     if (role_choose1 != 0)
    //     {
    //         for (int trip : Set_Trip_Role_parent[role_choose1])
    //         {
    //             childMark[trip] = 0;
    //         }
    //     }
    //     if (role_choose2 != 0)
    //     {
    //         for (int trip : Set_Trip_Role_parent[role_choose2])
    //         {
    //             childMark[trip] = 0;
    //         }
    //     }

    //     int id1 = getRandomNumber(0, Set_Trip_Role_parent[role_choose1].size() - 1);
    //     int id2 = getRandomNumber(0, Set_Trip_Role_parent[role_choose2].size() - 1);
    //     auto it1 = next(Set_Trip_Role_parent[role_choose1].begin(), id1);
    //     id1 = *it1;
    //     auto it2 = next(Set_Trip_Role_parent[role_choose2].begin(), id2);
    //     id2 = *it2;

    //     vector<int> temp;
    //     temp.push_back(0);
    //     for (int i = 1; i < childRoute[id1].size() - 1; ++i)
    //     {
    //         temp.push_back(childRoute[id1][i]);
    //     }
    //     for (int i = 1; i < childRoute[id2].size() - 1; ++i)
    //     {
    //         temp.push_back(childRoute[id2][i]);
    //     }
    //     temp.push_back(0);
    //     if (Set_Trip_Role_parent[role_choose2].size() == 1)
    //     {
    //         int index_big_role = *Set_Trip_Role_parent[role_choose2].begin();
    //         for (int _z = index_big_role + 1; _z < childRole.size(); _z++){
    //             childRole[_z]--;
    //         }
    //     }

    //     if (id1 > id2)
    //     {
    //         swap(id1, id2); // Đảm bảo xóa theo thứ tự đúng
    //     }
    //     childRoute.erase(childRoute.begin() + id2);
    //     childRole.erase(childRole.begin() + id2);
    //     childMark.erase(childMark.begin() + id2);

    //     childRoute.erase(childRoute.begin() + id1);
    //     childRole.erase(childRole.begin() + id1);
    //     childMark.erase(childMark.begin() + id1);

    //     childRoute.push_back(temp);
    //     childRole.push_back(role_choose1<role_choose2? role_choose1: role_choose2);
    //     childMark.push_back(0);
    //     mutate = true;
    // }
    //cout << " đang mutate :" << mutate << endl;
    // }
    // Tryindex++;
    // mutate = false;

    //cout << "sau mutate" << endl;
    // for (int b = 0; b < childRoute.size(); b++)
    // {
    //     for (int c = 0; c < childRoute[b].size(); c++)
    //     {
    //         cout << childRoute[b][c] << " ";
    //     }
    //     cout << childRole[b] << endl;
    // }
    // } while (Tryindex < 5 && !checkValidSolution(childRoute,childRole));

    // if (Tryindex < 15)
    //     return Solution(childRoute, childRole, childMark);

    // mutate = true;
    // return parents;
}

void Population::removeSame()
{
    unordered_set<Solution> unique_members(Mem.begin(), Mem.end());
    Mem.assign(unique_members.begin(), unique_members.end());
}

void Population::create_next_member(){
    vector<Solution> next_member;
    for (int i = 0; i < size;i++){
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
void Population::Genetic(string Method, double mutation_rate, int stagnation, bool Selectparent2)
{
    cout << "Start genetic" << endl;

    int batch_size = 5;
    int generation = 0;
    int index = 0;

    auto is_same_pareto = [](const vector<pair<int, double>> &a, const vector<pair<int, double>> &b)
    {
        if (a.size() != b.size())
            return false;
        for (const auto &p1 : a)
        {
            bool found = false;
            for (const auto &p2 : b)
            {
                if (p1.first == p2.first && fabs(p1.second - p2.second) < 1e-6)
                {
                    found = true;
                    break;
                }
            }
            if (!found)
                return false;
        }
        return true;
    };

    if (Method == "NSGA_II")
    {
        NSGA_II<Population, Solution> nsga_ii(*this);

        while (generation < stagnation)
        {
            cout << "Batch " << index++ << endl;
            nsga_ii.NSGA_II_Genetic(mutation_rate, batch_size,Selectparent2);

            generation += batch_size;

            // Nếu đủ dữ liệu để so sánh
            if (pareto_front_in_generation.size() < batch_size+1)
                continue;

            const auto &last_pareto = pareto_front_in_generation[pareto_front_in_generation.size() - batch_size];
            const auto &current_pareto = pareto_front_in_generation.back();

            if (is_same_pareto(last_pareto, current_pareto))
            {
                cout << "Pareto front không thay đổi - Tối ưu Tabu" << endl;
                for (int i = 0; i < current_pareto.size(); ++i)
                {
                    Mem[i].Tabu_Optimize(5, 3);
                }
            }
        }
    }
    else if (Method == "MOEA")
    {
        MOEA<Population, Solution> moea(*this);

        while (generation < stagnation)
        {
            cout << "Batch " << index++ << endl;
            moea.MOEA_Genetic(mutation_rate, batch_size,Selectparent2);

            generation += batch_size;

            if (pareto_front_in_generation.size() < 2)
                continue;

            const auto &last_pareto = pareto_front_in_generation[pareto_front_in_generation.size() - batch_size];
            const auto &current_pareto = pareto_front_in_generation.back();

            if (is_same_pareto(last_pareto, current_pareto))
            {
                cout << "Pareto front không thay đổi - Tối ưu Tabu" << endl;
                for (int i = 0; i < current_pareto.size(); ++i)
                {
                    Mem[i].Tabu_Optimize(5, 3);
                }
            }
        }
    }
}
