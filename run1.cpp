#include <iostream>
#include <vector>
#include <utility>   // pair
#include <string>
#include <cstdio>    // freopen
#include <algorithm>

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
    int N , M , K , J , P = 0;
    cin >> N >> M >> K >> J;
    if (scenario == 2) cin >> P;
    vector<pair<int , int>> start_points(K);
    vector<pair<int , int>> end_points(K);
    vector<pair<int , int>> popular_points(P);
    for (int i = 0; i < K; ++i) {
        cin >> start_points[i].first >> start_points[i].second >> end_points[i].first >> end_points[i].second;
    }
    if (scenario == 2) {
        for (int i = 0; i < P; ++i) {
            cin >> popular_points[i].first >> popular_points[i].second;
        }
    }
    // close input file
    fclose(stdin);
    vector<vector<int>> clauses;
    int num_vars = N*M*K + N*M*(K-1); // K paths, J colors, K-1 auxiliary variables


auto isEndpoint = [&](int i, int x, int y) {
    return (x == start_points[i].first && y == start_points[i].second) ||
           (x == end_points[i].first   && y == end_points[i].second);
};

    for (int i = 0; i < K; ++i) {
        vector<int> clause;
        pair<int, int> sp = start_points[i];
        pair<int, int> ep = end_points[i];
        // each path must start at its start point and end at its end point
        clause.push_back(i * N * M + sp.second * N + sp.first + 1);
        clauses.push_back(clause);
        clause.clear();
        clause.push_back((i * N * M + ep.second * N + ep.first + 1));
        clauses.push_back(clause);
        // no two paths can share the same start or end point
        for (int j = 0; j < K; ++j) {
            if (i != j) {
                vector<int> clause;
                clause.push_back(-(j * N * M + sp.second * N + sp.first + 1));
                clauses.push_back(clause);
                clause.clear();
                clause.push_back(-(j * N * M + ep.second * N + ep.first + 1));
                clauses.push_back(clause);
            }
        }
        clause.clear();
        vector<pair<int , int>> directions = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
        // at least one adjacent of start and end point
        for (auto dir : directions) {
            if ((sp.first + dir.second >= 0) && (sp.first + dir.second < N) && (sp.second + dir.first >= 0) && (sp.second + dir.first < M)) {
                clause.push_back((i * N * M + (sp.second + dir.first) * N + sp.first + dir.second + 1));
            }
        }
        clauses.push_back(clause);
        clause.clear();
        // at most one adjacent of start point
        for (int id1 = 0; id1 < 4; id1++) {
            for (int id2 = id1 + 1; id2 < 4; id2++) {
                vector<int> clause;
                if ((sp.first + directions[id1].second < 0) || (sp.first + directions[id1].second >= N) || (sp.second + directions[id1].first < 0) || (sp.second + directions[id1].first >= M)) continue;
                if ((sp.first + directions[id2].second < 0) || (sp.first + directions[id2].second >= N) || (sp.second + directions[id2].first < 0) || (sp.second + directions[id2].first >= M)) continue;
                clause.push_back(-(i * N * M + (sp.second + directions[id1].first) * N + sp.first + directions[id1].second + 1));
                clause.push_back(-(i * N * M + (sp.second + directions[id2].first) * N + sp.first + directions[id2].second + 1));
                clauses.push_back(clause);
            }
        }
        for (auto dir : directions) {
            if ((ep.first + dir.second >= 0) && (ep.first + dir.second < N) && (ep.second + dir.first >= 0) && (ep.second + dir.first < M)) {
                clause.push_back((i * N * M + (ep.second + dir.first) * N + ep.first + dir.second + 1));
            }
        }
        clauses.push_back(clause);
        clause.clear();
        // at most one adjacent of end point
        for (int id1 = 0; id1 < 4; id1++) {
            for (int id2 = id1 + 1; id2 < 4; id2++) {
                vector<int> clause;
                if ((ep.first + directions[id1].second < 0) || (ep.first + directions[id1].second >= N) || (ep.second + directions[id1].first < 0) || (ep.second + directions[id1].first >= M)) continue;
                if ((ep.first + directions[id2].second < 0) || (ep.first + directions[id2].second >= N) || (ep.second + directions[id2].first < 0) || (ep.second + directions[id2].first >= M)) continue;
                clause.push_back(-(i * N * M + (ep.second + directions[id1].first) * N + ep.first + directions[id1].second + 1));
                clause.push_back(-(i * N * M + (ep.second + directions[id2].first) * N + ep.first + directions[id2].second + 1));
                clauses.push_back(clause);
            }
        }
    }

        for (int x = 0; x< N; x++){
            for (int y = 0; y< M; y++){
                // each cell can be occupied by at most one path at a time using k-1 auxiliary variables
                bool endpoint = false;
                for (int i = 0; i<K; i++){
                    if (isEndpoint(i, x, y)){
                        endpoint = true;
                        break;
                    }
                }
                if (endpoint) continue;
                if (K>=2){
                    vector<int> clause;
                clause.push_back(-(y * N + x + 1));
                clause.push_back(N*M*K + (y * N + x + 1));
                clauses.push_back(clause);
                for (int i = 1; i<K-1; i++){
                    vector<int> clause;
                    clause.push_back(-(i*N*M + (y * N + x + 1)));
                    clause.push_back(N*M*K + i*N*M + (y * N + x + 1));
                    clauses.push_back(clause);
                    clause.clear();
                    clause.push_back(-(N*M*K + (i-1)*N*M + (y * N + x + 1)));
                    clause.push_back(N*M*K + i*N*M + (y * N + x + 1));
                    clauses.push_back(clause);
                    clause.clear();
                    clause.push_back(-(i*N*M + (y * N + x + 1)));
                    clause.push_back(-(N*M*K + (i-1)*N*M + (y * N + x + 1)));
                    clauses.push_back(clause);
                }
                clause.clear();
                clause.push_back(-( (K-1)*N*M + (y * N + x + 1)));
                clause.push_back(-(N*M*K + (K-2)*N*M + (y * N + x + 1)));
                clauses.push_back(clause);
                clause.clear();

                }
                
                // if a cell is occupied must have exactly two adjacent cells occupied
                for (int i =0; i<K; i++){
                    vector<pair<int , int>> directions = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
                    int h = 0;
                    int v = 0;
                    for (auto dir : directions) {
                        if ((x + dir.second >= 0) && (x + dir.second < N) && (y + dir.first >= 0) && (y + dir.first < M)) {
                            if (dir.first == 0) h++;
                            else v++;
                        }
                    }
                    if (h+v == 1){
                        vector<int> clause;
                        clause.push_back(-(N * M * i + y * N + x + 1));
                        clauses.push_back(clause);
                        continue;
                    }
                    if (h+v == 2) {
                        for (auto dir : directions) {
                            if ((x + dir.second < 0) || (x + dir.second >= N) || (y + dir.first < 0) || (y + dir.first >= M)) continue;
                            vector<int> clause;
                            clause.push_back(-(N * M * i + y * N + x + 1));
                            clause.push_back(N * M * i + (y + dir.first) * N + x + dir.second + 1);
                            clauses.push_back(clause);
                        }
                        cout << "hi" << endl;
                        continue;
                    }
                    if (h+v == 3) {
                        vector<int> clause;
                        clause.push_back(-(N * M * i + y * N + x + 1));
                        for (int id1 = 0; id1 < 4; id1++) {
                            if ((x + directions[id1].second < 0) || (x + directions[id1].second >= N) || (y + directions[id1].first < 0) || (y + directions[id1].first >= M)) continue;
                            clause.push_back(-(N * M * i + (y + directions[id1].first) * N + x + directions[id1].second + 1));
                            for (int id2 = id1 + 1; id2 < 4; id2++) {
                                vector<int> clause2;
                                clause2.push_back(-(N * M * i + y * N + x + 1));
                                if ((x + directions[id2].second < 0) || (x + directions[id2].second >= N) || (y + directions[id2].first < 0) || (y + directions[id2].first >= M)) continue;
                                clause2.push_back((N * M * i + (y + directions[id1].first) * N + x + directions[id1].second + 1));
                                clause2.push_back((N * M * i + (y + directions[id2].first) * N + x + directions[id2].second + 1));
                                clauses.push_back(clause2);
                            }
                        }
                        clauses.push_back(clause);
                        continue;
                    }
                vector<int> clause;
                clause.push_back(-(N * M * i + y * N + x + 1));
                for (int id1 = 0; id1 < 4; id1++) {
                    vector<int> clause2;
                    vector<int> clause3;
                    clause.push_back(N * M * i + (y + directions[id1].first) * N + x + directions[id1].second + 1);
                    clause2.push_back(-(N * M * i + (y + directions[id1].first) * N + x + directions[id1].second + 1));
                    for (int id2 = 0; id2 < 4; id2++) {
                        if (id1 == id2) continue;
                        clause2.push_back(N * M * i + (y + directions[id2].first) * N + x + directions[id2].second + 1);
                        clause3.push_back(-(N * M * i + (y + directions[id2].first) * N + x + directions[id2].second + 1));
                    }
                    clause2.push_back(-(N * M * i + y * N + x + 1));
                    clause3.push_back(-(N * M * i + y * N + x + 1));
                    clauses.push_back(clause2);
                    clauses.push_back(clause3);
                }
                clauses.push_back(clause);
                clause.clear();
                }

                
            }
        }
    
auto addClause = [&](std::initializer_list<int> C) {
    clauses.emplace_back(C);
};
auto addClauseVec = [&](const std::vector<int>& C) {
    clauses.push_back(C);
};
auto Xvar = [&](int i, int x, int y) {
    return i * N * M + y * N + x + 1;
};

int baseT = num_vars;                       // first new var id will be baseT + 1
auto Tvar = [&](int i, int x, int y) {
    return baseT + (i * N * M + y * N + x + 1); // 1-based inside the block
};
num_vars += K * N * M;                      // reserve K*N*M turn literals

// Build local definition of "turn at (i,x,y)" using a single T literal.
// T_{i,x,y}  <->  X_{i,x,y} ∧ (L ∨ R) ∧ (U ∨ D)  (with out-of-bounds removed)
// And the four corner implications: (¬L ∨ ¬U ∨ ¬X ∨ T), etc.
for (int i = 0; i < K; ++i) {
    for (int y = 0; y < M; ++y) {
        for (int x = 0; x < N; ++x) {


            const int X = Xvar(i, x, y);
            const int T = Tvar(i, x, y);
            if (isEndpoint(i, x, y)) {
                addClause({-T});
                continue;
            }
            // Collect in-bounds neighbor literals (same path i)
            std::vector<int> H; // horizontal neighbors
            std::vector<int> V; // vertical neighbors
            if (x - 1 >= 0) H.push_back(Xvar(i, x - 1, y));
            if (x + 1 <  N) H.push_back(Xvar(i, x + 1, y));
            if (y - 1 >= 0) V.push_back(Xvar(i, x, y - 1));
            if (y + 1 <  M) V.push_back(Xvar(i, x, y + 1));

            // If there is no possible turn geometry (no H or no V), forbid T directly.
            if (H.empty() || V.empty()) {
                addClause({-T}); // can't turn on a line with only one axis available
                continue;
            }

            // T ⇒ X,  T ⇒ (H-or),  T ⇒ (V-or)
            addClause({-T, X});
            {
                std::vector<int> C; 
                C.push_back(-T);
                for (int h : H) C.push_back(h);
                addClauseVec(C);
            }
            {
                std::vector<int> C;
                C.push_back(-T);
                for (int v : V) C.push_back(v);
                addClauseVec(C);
            }

            // (h ∧ v ∧ X) ⇒ T  as  (¬h ∨ ¬v ∨ ¬X ∨ T), for all h in H, v in V
            for (int h : H) {
                for (int v : V) {
                    addClause({-h, -v, -X, T});
                }
            }

            // (Optional) If you know endpoints must have degree 1, you can tighten by forbidding T there:
        }
    }
}

// === At-most-J turns per path using a sequential counter ====================
// We gather the T's per path and enforce sum(T) <= J.
// Special-cases: J >= m (no constraint), J == 0 (just set all T false).

auto newVar = [&]() -> int { return ++num_vars; };

for (int i = 0; i < K; ++i) {
    std::vector<int> Ti;
    for (int y = 0; y < M; ++y) {
        for (int x = 0; x < N; ++x) {
            // We included all cells above; T at cells with no turn geometry are already forced false.
            // You may skip endpoints here (they were forced false too).
            Ti.push_back(Tvar(i, x, y));
        }
    }

    const int m = (int)Ti.size();
    if (m == 0) continue;
    if (J >= m) continue;        // trivially satisfied
    if (J == 0) {                // no turns allowed
        for (int t : Ti) addClause({-t});
        continue;
    }

    // Sequential AtMost-J counter (Bailleux–Boufkhad style).
    // S[p][q] means: among the first p inputs, at least q are true.
    std::vector<std::vector<int>> S(m + 1, std::vector<int>(J + 1, 0));
    for (int p = 1; p <= m; ++p)
        for (int q = 1; q <= J; ++q)
            S[p][q] = newVar();

    // (¬t_p ∨ S_{p,1})  for p=1..m
    for (int p = 1; p <= m; ++p) addClause({-Ti[p-1], S[p][1]});

    // (¬S_{p-1,q} ∨ S_{p,q})  for p=2..m, q=1..J
    for (int p = 2; p <= m; ++p)
        for (int q = 1; q <= J; ++q)
            addClause({-S[p-1][q], S[p][q]});

    // (¬t_p ∨ ¬S_{p-1,J})  for p=2..m
    for (int p = 2; p <= m; ++p)
        addClause({-Ti[p-1], -S[p-1][J]});

    // (¬t_p ∨ ¬S_{p-1,q-1} ∨ S_{p,q})  for p=2..m, q=2..J
    for (int p = 2; p <= m; ++p)
        for (int q = 2; q <= J; ++q)
            addClause({-Ti[p-1], -S[p-1][q-1], S[p][q]});
}

    int cur = 1;

    string output_path = file_path.substr(0, file_path.find_last_of('.')) + ".satinput";
    freopen(output_path.c_str(), "w", stdout);
    // p cnf variables clauses 
    // each clause as 1 indexed variables
    cout << "p cnf " << num_vars << " " << clauses.size() << endl;
    for (const auto& clause : clauses) {
        for (int lit : clause) {
            cout << lit << " ";
        }
        cout << "0" << endl;
    }
    fclose(stdout);
    return 0;
}


//​

// (A∨B∨C∨D)∧(¬A∨B∨C∨D)∧(¬B∨A∨C∨D)∧
// (¬C∨A∨B∨D)∧(¬D∨A∨B∨C)∧(¬A∨¬B∨¬C)∧
// (¬A∨¬B∨¬D)∧(¬A∨¬C∨¬D)∧(¬B∨¬C∨¬D).
// 	​


// This 9-clause CNF is satisfiable iff exactly two of 
// 𝐴
// ,
// 𝐵
// ,
// 𝐶
// ,
// 𝐷
// A,B,C,D are true.
