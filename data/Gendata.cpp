#include <bits/stdc++.h>
#include <random>
 
using namespace std;
struct Point
{
    double x, y;
};

double EuclideanDistance(const Point &a, const Point &b)
{
    return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

vector<vector<double>> GenDistanceMatrix(int n, double min_coord, double max_coord)
{
    vector<Point> points(n);
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<double> dis(min_coord, max_coord);

    // Sinh tọa độ ngẫu nhiên cho các điểm
    for (int i = 0; i < n; ++i)
    {
        points[i].x = dis(gen);
        points[i].y = dis(gen);
    }

    // Tạo ma trận khoảng cách
    vector<vector<double>> distance_matrix(n, vector<double>(n, 0.0));
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            if (i != j)
                distance_matrix[i][j] = EuclideanDistance(points[i], points[j]);
        }
    }

    return distance_matrix;
}

struct Customer
{
    double start, end;
    double demand;
    int role;
};

vector<Customer> GenCustomer(int n, double min_start, double max_start, double min_end, double max_end, double min_demand, double max_demand)
{
    vector<Customer> customers(n);
    random_device rd;
    mt19937 gen(rd());


    // Sinh thông tin cho các khách hàng
    for (int i = 0; i < n; ++i)
    {
        uniform_real_distribution<double> dis_start(min_start, max_start);
        double start = dis_start(gen);
        uniform_real_distribution<double> dis_end(start, max_end);
        double end = dis_end(gen);
        uniform_real_distribution<double> dis_percent(0,1);
        double percent = dis_percent(gen);
        if(percent<0.86){
            uniform_real_distribution<double> dis_demand(min_demand, 2.27);
            customers[i].demand = dis_demand(gen);
        }
        else {
            uniform_real_distribution<double> dis_demand(2.27, max_demand);
            customers[i].demand = dis_demand(gen);
        }

        customers[i].start = start;
        customers[i].end = end;

        double role_percent = dis_percent(gen);
        if (role_percent < 0.8)
            customers[i].role = 0; // Khách hàng bình thường
        else
            customers[i].role = 1; // Khách hàng đặc biệt
    }

    return customers;
}


int main(){
    vector<int> num_customers = {20, 50, 100, 200};
    vector<vector<double>> grid ={{5,10,20},{10,20,30,40},{10,20,30,40},{10,20,30,40}};
    vector<pair<int, int>> td = {{2,2},{4,6},{6,9},{8,12}};
    
    for(int i = 0; i < num_customers.size(); ++i)
    {
        for(double x : grid[i]){
            for (int j = 1; j <= 4;j++){
                string file_name;
                file_name = "D:\\Project GA new\\data\\Data_" + to_string(num_customers[i]) + "_" + to_string(int(x)) + "_" + to_string(j) + ".txt";
                freopen(file_name.c_str(), "w", stdout);
                int n = num_customers[i];
                double min_coord = 0.0;
                double max_coord = x;

                double min_start = 0.0;
                double max_start = 19.0;

                double min_end = 0.0;
                double max_end = 24.0;
                double min_demand = 0.2;
                double max_demand = 30;

                vector<vector<double>> distance_matrix = GenDistanceMatrix(n + 1, min_coord, max_coord);
                vector<Customer> customers = GenCustomer(n, min_start, max_start, min_end, max_end, min_demand, max_demand);

                double Max_weight_truck = 400;
                double Max_weight_drone = 50;
                double Max_energy_drone = 500;

                double t_drone = 0.0125;
                double t_truck = 0.025;

                double cost_drone = 300000;
                double cost_truck = 1000000;

                double en_drone = 15;

                double k_truck = 10000;
                double k_drone = 3000;

                cout << n << endl;
                for (int i = 0; i < n; ++i)
                {
                    cout << customers[i].start << " " << customers[i].end << " " << customers[i].demand << " " << customers[i].role << endl;
                }

                cout << Max_weight_truck << " " << Max_weight_drone << " " << Max_energy_drone << endl;
                cout << t_truck << " " << t_drone << " " << k_truck << " " << k_drone << " " << en_drone << " " << cost_truck << " " << cost_drone << " ";
                cout << td[i].first << " " << td[i].second << endl;

                for (int i = 0; i < n + 1; ++i)
                {
                    for (int j = 0; j < n + 1; ++j)
                    {
                        cout << distance_matrix[i][j] << " ";
                    }
                    cout << endl;
                }
            }
        }
        
    }
    
    return 0;

}
