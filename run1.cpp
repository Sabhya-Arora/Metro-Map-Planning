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
    vector<pair<int , int>> end_points(J);
    for (int i = 0; i < K; ++i) {
        cin >> start_points[i].first >> start_points[i].second >> end_points[i].first >> end_points[i].second;
    }











    string output_path = file_path.substr(0, file_path.find_last_of('.')) + ".satinput";
    freopen(output_path.c_str(), "w", stdout);
    // p cnf variables clauses 
    // each clause as 1 indexed variables

}