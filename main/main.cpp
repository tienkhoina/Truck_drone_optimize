#include <iostream>
#include <fstream>
#include <limits>
#include "TSPTW.h"

using namespace std;

int main()
{
    try
    {
        // Đọc dữ liệu từ file
        ifstream fin("D:\\Project GA new\\data\\TSPTW.txt");
        if (!fin)
        {
            cerr << "Cannot open input file" << endl;
            return 1;
        }

        int n;
        fin >> n;

        // Kiểm tra giá trị n hợp lệ
        if (n <= 0 || n > 1000)
        { // Giả sử giới hạn 1000 điểm
            cerr << "Invalid number of customers: " << n << endl;
            return 1;
        }

        vector<double> e(n + 1), l(n + 1), d(n + 1);
        e[0] = 0;
        l[0] = numeric_limits<double>::max(); // Sử dụng giá trị max thay vì 1e9
        d[0] = 0;

        // Đọc dữ liệu với kiểm tra lỗi
        for (int i = 1; i <= n; ++i)
        {
            if (!(fin >> e[i] >> l[i] >> d[i]))
            {
                cerr << "Error reading time windows for customer " << i << endl;
                return 1;
            }
            // Kiểm tra giá trị hợp lệ
            if (e[i] < 0 || l[i] < e[i] || d[i] < 0)
            {
                cerr << "Invalid time window for customer " << i << endl;
                return 1;
            }
        }

        vector<vector<double>> C(n + 1, vector<double>(n + 1));
        for (int i = 0; i <= n; ++i)
        {
            for (int j = 0; j <= n; ++j)
            {
                if (!(fin >> C[i][j]))
                {
                    cerr << "Error reading cost matrix at [" << i << "][" << j << "]" << endl;
                    return 1;
                }
                if (C[i][j] < 0)
                {
                    cerr << "Invalid cost at [" << i << "][" << j << "]" << endl;
                    return 1;
                }
            }
        }

        // Kiểm tra xem đã đọc hết file chưa
        string extra;
        if (fin >> extra)
        {
            cerr << "Warning: Extra data in input file" << endl;
        }

        cout << "Problem with " << n << " customers loaded successfully." << endl;

        // Giải bài toán
        TSPTW tsp(n, C, e, l, d);
        tsp.solve();

        // Hiển thị kết quả
        vector<int> solution = tsp.getSolution();

        if (solution.empty() || solution.size() < 2)
        {
            cout << "No valid solution found" << endl;
            return 1;
        }

        cout << "Optimal route: 0";
        for (size_t i = 1; i < solution.size() && solution[i] != 0; ++i)
        {
            cout << " -> " << solution[i];
        }
        cout << " -> 0" << endl;
    }
    catch (const exception &e)
    {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}