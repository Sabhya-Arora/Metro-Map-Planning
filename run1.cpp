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
    string file_base = argv[1];
    string file_path = file_base.substr(0, file_base.find_last_of('.')) + ".city";
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
    // bool is_break = false;
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
        if (scenario == 1) {
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
                    if (K<30){
                        for (int i = 0; i<K; i++){
                            for (int j = i+1; j<K; j++){
                                vector<int> clause;
                                clause.push_back(-(i*N*M + (y * N + x + 1)));
                                clause.push_back(-(j*N*M + (y * N + x + 1)));
                                clauses.push_back(clause);
                            }
                        }
                    }
                    else {
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
                   

                }
                if (scenario == 1) {
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
                            // cout << "hi" << endl;
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

if (scenario == 2) {
// Directions: 0:Up, 1:Down, 2:Left, 3:Right

// === 1. INITIALIZE VARIABLES ===
    int base_X = 0;
    int base_Out = baseT + K*N*M;
    num_vars = base_Out + 4 * K * N * M;
auto Outvar = [&](int k, int x, int y, int dir) {
    return base_Out + k * N * M * 4 + y * N * 4 + x * 4 + dir + 1;
};

auto isStartOrEndPoint = [&](int x, int y, int& line_idx, bool& is_start) {
    for (int i = 0; i < K; ++i) {
        if (x == start_points[i].first && y == start_points[i].second) {
            line_idx = i;
            is_start = true;
            return true;
        }
        if (x == end_points[i].first && y == end_points[i].second) {
            line_idx = i;
            is_start = false;
            return true;
        }
    }
    return false;
};
    // Directions: U, D, L, R
    const vector<pair<int, int>> dirs = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};
    const vector<int> opp_dir_idx = {1, 0, 3, 2};


    // === 2. GENERATE CLAUSES ===

    // --- Link Occupancy (X) and Outflow (Out) ---
    for (int k = 0; k < K; ++k) {
        for (int y = 0; y < M; ++y) {
            for (int x = 0; x < N; ++x) {
                bool is_end = (x == end_points[k].first && y == end_points[k].second);
                vector<int> out_vars;
                for (int d = 0; d < 4; ++d) {
                    addClause({-Outvar(k, x, y, d), Xvar(k, x, y)}); // outflow implies occupancy
                    out_vars.push_back(Outvar(k, x, y, d));
                }
                if (!is_end) {
                    out_vars.push_back(-Xvar(k, x, y)); // occupancy implies at least one outflow
                    addClauseVec(out_vars);
                }
            }
        }
    }

    // --- Path, Flow, and Uniqueness Constraints ---
    for (int y = 0; y < M; ++y) {
        for (int x = 0; x < N; ++x) {
            int endpoint_k;
            bool is_start_flag;
            bool is_ep = isStartOrEndPoint(x, y, endpoint_k, is_start_flag);

            for (int k = 0; k < K; ++k) {
                vector<int> inflows;
                for(int d=0; d<4; ++d){
                    int nx = x + dirs[d].first;
                    int ny = y + dirs[d].second;
                    if(nx >= 0 && nx < N && ny >= 0 && ny < M){
                        inflows.push_back(Outvar(k, nx, ny, opp_dir_idx[d]));
                    }
                }
                
                vector<int> outflows;
                 for(int d=0; d<4; ++d) outflows.push_back(Outvar(k, x, y, d));


                if (is_ep && k == endpoint_k) { // START or END point
                    if (is_start_flag) { // START
                        for(int inflow_var : inflows) addClause({-inflow_var}); // No inflow
                        addClauseVec(outflows); // At least one outflow
                        for(int i=0; i<4; ++i) for(int j=i+1; j<4; ++j) addClause({-outflows[i], -outflows[j]}); // At most one outflow
                    } else { // END
                        for(int outflow_var : outflows) addClause({-outflow_var}); // No outflow
                        addClauseVec(inflows); // At least one inflow
                        for(size_t i=0; i<inflows.size(); ++i) for(size_t j=i+1; j<inflows.size(); ++j) addClause({-inflows[i], -inflows[j]}); // At most one inflow
                    }
                } else { // INTERMEDIATE point
                    // Flow conservation
                    for(int inflow_var : inflows) addClause({-inflow_var, Xvar(k,x,y)}); // inflow implies occupancy
                    
                    vector<int> clause_inf = {-Xvar(k,x,y)};
                    for(int v : inflows) clause_inf.push_back(v);
                    addClauseVec(clause_inf); // occupancy implies at least one inflow
                    
                    // At most one inflow/outflow
                    for(size_t i=0; i<inflows.size(); ++i) for(size_t j=i+1; j<inflows.size(); ++j) addClause({-inflows[i], -inflows[j], -Xvar(k,x,y)});
                    for(int i=0; i<4; ++i) for(int j=i+1; j<4; ++j) addClause({-outflows[i], -outflows[j], -Xvar(k,x,y)});

                    // No U-turns
                    if(y > 0) addClause({-Outvar(k, x, y-1, 1), -Outvar(k, x, y, 0)}); // In-Down, Out-Up
                    if(y < M-1) addClause({-Outvar(k, x, y+1, 0), -Outvar(k, x, y, 1)}); // In-Up, Out-Down
                    if(x > 0) addClause({-Outvar(k, x-1, y, 3), -Outvar(k, x, y, 2)}); // In-Right, Out-Left
                    if(x < N-1) addClause({-Outvar(k, x+1, y, 2), -Outvar(k, x, y, 3)}); // In-Left, Out-Right
                }
            }
        }
    }

    // // --- Turn Constraints ---
    for (int k = 0; k < K; ++k) {
        vector<int> path_turn_vars;
        for (int y = 0; y < M; ++y) {
            for (int x = 0; x < N; ++x) {
                int T = Tvar(k, x, y);
                path_turn_vars.push_back(T);
                addClause({-T, Xvar(k, x, y)});
                bool is_ep_k = (x == start_points[k].first && y == start_points[k].second) || (x == end_points[k].first && y == end_points[k].second);
                if(is_ep_k) {
                    addClause({-T}); // No turns at endpoints
                    continue;
                }

                vector<int> H_in, V_in;
                if (x > 0) H_in.push_back(Outvar(k, x - 1, y, 3));
                if (x < N - 1) H_in.push_back(Outvar(k, x + 1, y, 2));
                if (y > 0) V_in.push_back(Outvar(k, x, y - 1, 1));
                if (y < M - 1) V_in.push_back(Outvar(k, x, y + 1, 0));

                vector<int> H_out = {Outvar(k, x, y, 2), Outvar(k, x, y, 3)};
                vector<int> V_out = {Outvar(k, x, y, 0), Outvar(k, x, y, 1)};

                // T <=> (H_in and V_out) or (V_in and H_out)
                for (int h_in : H_in) for (int v_out : V_out) {
                    addClause({-h_in, -v_out, T});
                }
                for (int v_in : V_in) for (int h_out : H_out) {
                    addClause({-v_in, -h_out, T});
                }
                // Clause A: T => (Any_H_in ∨ Any_H_out)
                vector<int> t_implies_h_flow = {-T};
                t_implies_h_flow.insert(t_implies_h_flow.end(), H_in.begin(), H_in.end());
                t_implies_h_flow.insert(t_implies_h_flow.end(), H_out.begin(), H_out.end());
                if (t_implies_h_flow.size() > 1) { // Only add if horizontal flow is possible
                    addClauseVec(t_implies_h_flow);
                }

                // Clause B: T => (Any_V_in ∨ Any_V_out)
                vector<int> t_implies_v_flow = {-T};
                t_implies_v_flow.insert(t_implies_v_flow.end(), V_in.begin(), V_in.end());
                t_implies_v_flow.insert(t_implies_v_flow.end(), V_out.begin(), V_out.end());
                if (t_implies_v_flow.size() > 1) { // Only add if vertical flow is possible
                    addClauseVec(t_implies_v_flow);
                }
                
            }
        }

    }
    
    // --- Popular Cell Constraints (Scenario 2) ---
    if (scenario == 2) {
        for (const auto& p_point : popular_points) {
            vector<int> popular_clause;
            for (int k = 0; k < K; ++k) {
                popular_clause.push_back(Xvar(k, p_point.first, p_point.second));
            }
            if (!popular_clause.empty()) {
                addClauseVec(popular_clause);
            }
        }
    }


}

num_vars += K * N * M;                      // reserve K*N*M turn literals

// Build local definition of "turn at (i,x,y)" using a single T literal.
// T_{i,x,y}  <->  X_{i,x,y} ∧ (L ∨ R) ∧ (U ∨ D)  (with out-of-bounds removed)
// And the four corner implications: (¬L ∨ ¬U ∨ ¬X ∨ T), etc.
if (scenario == 1) 
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

if (J != 1 && J!=2){

auto newVar = [&]() -> int { return ++num_vars; };

for (int i = 0; i < K; ++i) {
    std::vector<int> Ti;
    for (int y = 0; y < M; ++y) {
        for (int x = 0; x < N; ++x) {
            if (J == 0){
                continue;
            }
            if (J == 2){
                if ((x == start_points[i].first )|| (y == start_points[i].second) || (x == end_points[i].first) || (y == end_points[i].second))             Ti.push_back(Tvar(i, x, y));
                continue;

            }
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
}
for (int i = 0; i<K; i++){
        if (J == 0){
            if ((start_points[i].first != end_points[i].first) && (start_points[i].second != end_points[i].second)){
                // if start and end points are not aligned, path must turn at least once
                clauses.clear();
                clauses.push_back({1});
                clauses.push_back({-1});
                break;
                // is_break = true;
            }
            else {
                for (int x = 0; x < N; ++x){
                    for (int y = 0; y < M; y++){
                        if (start_points[i].first == end_points[i].first){
                            if (x != start_points[i].first){
                                clauses.push_back({-(i*N*M + (y * N + x + 1))});
                            }
                            else {
                                if ((y < min(start_points[i].second, end_points[i].second)) || (y > max(start_points[i].second, end_points[i].second))){
                                    clauses.push_back({-(i*N*M + (y * N + x + 1))});
                                }
                            }
                        }
                        else {
                            if (y != start_points[i].second){
                                clauses.push_back({-(i*N*M + (y * N + x + 1))});
                            }
                            else {
                                if ((x < min(start_points[i].first, end_points[i].first)) || (x > max(start_points[i].first, end_points[i].first))){
                                    clauses.push_back({-(i*N*M + (y * N + x + 1))});
                                }
                            }
                        }
                    }
                }
                
            }
        }
        else if (J == 1){
            for (int x = 0; x<N; x++){
                for (int y = 0; y<M; y++){
                    if ((x!= start_points[i].first)&& (x!= end_points[i].first) && (y!= start_points[i].second) && (y!= end_points[i].second)){
                        clauses.push_back({-(i*N*M + (y * N + x + 1))});
                    }
                    else if ((x == start_points[i].first) || (x == end_points[i].first)){
                        if ((y < min(start_points[i].second, end_points[i].second)) || (y > max(start_points[i].second, end_points[i].second))){
                            clauses.push_back({-(i*N*M + (y * N + x + 1))});
                        }
                        else if ((y > min(start_points[i].second , end_points[i].second)) && (y < max(start_points[i].second , end_points[i].second))){
                            clauses.push_back({-Tvar(i, x, y)});
                        }
                    }
                    else if ((y == start_points[i].second) || (y == end_points[i].second)){
                        if ((x < min(start_points[i].first, end_points[i].first)) || (x > max(start_points[i].first, end_points[i].first))){
                            clauses.push_back({-(i*N*M + (y * N + x + 1))});
                        }
                        else if ((x > min(start_points[i].first , end_points[i].first)) && (x < max(start_points[i].first , end_points[i].first))){
                            clauses.push_back({-Tvar(i, x, y)});}
                    }
                }
            }
        }
        else if (J == 2){
            int max_clauses = min(num_vars*20 , 2000000);
            bool is_break = false;
            for (int x = 0; x<N; x++){
                // if (clauses.size() > max_clauses){
                //     break;
                // }
                for (int y = 0; y<M; y++){
                    // if (clauses.size() > max_clauses){
                    //     break;
                    // }
                    if (x < min(start_points[i].first , end_points[i].first) && y < min(start_points[i].second , end_points[i].second)){
                        clauses.push_back({-(i*N*M + (y * N + x + 1))});
                    }
                    else if (x < min(start_points[i].first , end_points[i].first) && y > max(start_points[i].second , end_points[i].second)){
                        clauses.push_back({-(i*N*M + (y * N + x + 1))});
                    }
                    else if (x > max(start_points[i].first , end_points[i].first) && y < min(start_points[i].second , end_points[i].second)){
                        clauses.push_back({-(i*N*M + (y * N + x + 1))});
                    }
                    else if (x > max(start_points[i].first , end_points[i].first) && y > max(start_points[i].second , end_points[i].second)){
                        clauses.push_back({-(i*N*M + (y * N + x + 1))});
                    }
                }
            }
            vector<pair<int, int>> turn_points;
            for (int x = 0; x<N; x++){
                for (int y = 0; y<M; y++){
                    if (x == start_points[i].first && y == start_points[i].second) continue;
                    if (x == end_points[i].first && y == end_points[i].second) continue;
                    if ((x != start_points[i].first) && (x != end_points[i].first) && (y != start_points[i].second) && (y != end_points[i].second)){
                        clauses.push_back({-Tvar(i, x, y)});
                    }
                    else{
                        turn_points.push_back({x, y});
                    }
                }
            }
            for (int p1 = 0; p1 < turn_points.size(); p1++){
                for (int p2 = 0; p2 < turn_points.size(); p2++){
                    if (p1 == p2) continue;
                    int x1 = turn_points[p1].first;
                    int y1 = turn_points[p1].second;
                    int x2 = turn_points[p2].first;
                    int y2 = turn_points[p2].second;
                    vector<int> clause;
                if (x1 == start_points[i].first){
                        if (y1== end_points[i].second){
                            vector<int> clause;
                            clause.push_back(-Tvar(i , x1 , y1));
                            clause.push_back(-Tvar(i , x2 , y2));
                            clauses.push_back(clause);
                        }
                        else {
                            if (y2 == y1){
                                vector<int> clause;
                                clause.push_back(-Tvar(i , x1 , y1));
                                clause.push_back(Tvar(i , x2 , y2));
                                clauses.push_back(clause);
                            }
                            else{
                                vector<int> clause;
                                clause.push_back(-Tvar(i , x1 , y1));
                                clause.push_back(-Tvar(i , x2 , y2));
                                clauses.push_back(clause);
                            }
                        }
                }
                else if (x1 == end_points[i].first){
                        if (y1== start_points[i].second){
                            vector<int> clause;
                            clause.push_back(-Tvar(i , x1 , y1));
                            clause.push_back(-Tvar(i , x2 , y2));
                            clauses.push_back(clause);
                        }
                        else {
                            if (y2 == y1){
                                vector<int> clause;
                                clause.push_back(-Tvar(i , x1 , y1));
                                clause.push_back(Tvar(i , x2 , y2));
                                clauses.push_back(clause);
                            }
                            else{
                                vector<int> clause;
                                clause.push_back(-Tvar(i , x1 , y1));
                                clause.push_back(-Tvar(i , x2 , y2));
                                clauses.push_back(clause);
                            }
                        }
                }
                else if (y1 == start_points[i].second){
                        if (x1== end_points[i].first){
                            vector<int> clause;
                            clause.push_back(-Tvar(i , x1 , y1));
                            clause.push_back(-Tvar(i , x2 , y2));
                            clauses.push_back(clause);
                        }
                        else {
                            if (x2 == x1){
                                vector<int> clause;
                                clause.push_back(-Tvar(i , x1 , y1));
                                clause.push_back(Tvar(i , x2 , y2));
                                clauses.push_back(clause);
                            }
                            else{
                                vector<int> clause;
                                clause.push_back(-Tvar(i , x1 , y1));
                                clause.push_back(-Tvar(i , x2 , y2));
                                clauses.push_back(clause);
                            }
                        }
                }
                else if (y1 == end_points[i].second){
                        if (x1== start_points[i].first){
                            vector<int> clause;
                            clause.push_back(-Tvar(i , x1 , y1));
                            clause.push_back(-Tvar(i , x2 , y2));
                            clauses.push_back(clause);
                        }
                        else {
                            if (x2 == x1){
                                vector<int> clause;
                                clause.push_back(-Tvar(i , x1 , y1));
                                clause.push_back(Tvar(i , x2 , y2));
                                clauses.push_back(clause);
                            }
                            else{
                                vector<int> clause;
                                clause.push_back(-Tvar(i , x1 , y1));
                                clause.push_back(-Tvar(i , x2 , y2));
                                clauses.push_back(clause);
                            }
                        }
                }
            }
        }
    }
    else if (J == 3){
        for (int i = 0; i < K; i++) {
            int xs = start_points[i].first;
            int ys = start_points[i].second;
            for (int y1 = 0; y1 < M; y1++) {
                for (int y2 = y1 + 1; y2 < M; y2++) {
                    clauses.push_back({-Tvar(i, xs, y1), -Tvar(i, xs, y2)});
                }
            }
            for (int x1 = 0; x1 < N; x1++) {
                for (int x2 = x1 + 1; x2 < N; x2++) {
                    clauses.push_back({-Tvar(i, x1, ys), -Tvar(i, x2, ys)});
                }
            }
            int xe = end_points[i].first;
            int ye = end_points[i].second;
            for (int y1 = 0; y1 < M; y1++) {
                for (int y2 = y1 + 1; y2 < M; y2++) {
                    clauses.push_back({-Tvar(i, xe, y1), -Tvar(i, xe, y2)});
                }
            }
            for (int x1 = 0; x1 < N; x1++) {
                for (int x2 = x1 + 1; x2 < N; x2++) {
                    clauses.push_back({-Tvar(i, x1, ye), -Tvar(i, x2, ye)});
                }
            }
            for (int x = 0; x < N; x++) {
                for (int y = 0; y < M; y++) {
                    // Skip start or end cells
                    if ((x == xs && y == ys) || (x == xe && y == ye)) continue;

                    // Only for off-grid-line turns
                    if ((x != xs) && (x != xe) && (y != ys) && (y != ye)) {
                        int Txy  = Tvar(i, x, y);
                        int A = Tvar(i, x, ys);  // same row as start
                        int B = Tvar(i, xe, y);  // same column as end
                        int C = Tvar(i, xs, y);  // same column as start
                        int D = Tvar(i, x, ye);  // same row as end

                        // (¬Txy ∨ A ∨ C)
                        clauses.push_back({-Txy, A, C});

                        // (¬Txy ∨ A ∨ D)
                        clauses.push_back({-Txy, A, D});

                        // // (¬Txy ∨ B ∨ C)
                        clauses.push_back({-Txy, B, C});

                        // (¬Txy ∨ B ∨ D)
                        clauses.push_back({-Txy, B, D});
                    }
                }
            }
        }
            
    }
}
    int cur = 1;

    string output_path = file_base + ".satinput";
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