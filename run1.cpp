#include <bits/stdc++.h>

using namespace std;

int main(int argc, char* argv[]) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <file path>" << endl;
        return 1;
    }
    string file_path = argv[1];
    freopen(file_path.c_str(), "r", stdin);

    int scenario;
    cin >> scenario;
    int N , M , K , J;
    cin >> N >> M >> K >> J;
    vector<pair<int , int>> start_points(K);
    vector<pair<int , int>> end_points(K);
    for (int i = 0; i < K; ++i) {
        cin >> start_points[i].first >> start_points[i].second >> end_points[i].first >> end_points[i].second;
    }









    int cur = 1;

    string output_path = file_path.substr(0, file_path.find_last_of('.')) + ".satinput";
    freopen(output_path.c_str(), "w", stdout);
    // p cnf variables clauses 
    // each clause as 1 indexed variables followed by 0
    cout << "p cnf " << (N * M * J + K * J) << " " << (N * M * J * 5 + K * J * 3 + K * (K - 1) / 2 * J * J) << "\n";
    vector<vector<vector<int>>> cell_occupied(N, vector<vector<int>>(M, vector<int>(K, 0))), turn(N, vector<vector<int>>(M, vector<int>(K, 0)));
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            for (int k = 0; k < K; k++) {
                cell_occupied[i][j][k] = cur++;
            }
        }
    }
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            for (int k = 0; k < K; k++) {
                turn[i][j][k] = cur++;
            }
        }
    }
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            for (int k = 0; k < K; k++) {
                for (int k2 = k + 1; k2 < K; k2++) {
                    cout<< -cell_occupied[i][j][k] << " " << -cell_occupied[i][j][k2] << " 0\n";
                }
            }
        }
    }
    
}

