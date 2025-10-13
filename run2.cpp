// run2.cpp
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <queue>
#include <algorithm>
using namespace std;

static inline int Xvar(int i,int x,int y,int N,int M){ return i*N*M + y*N + x + 1; }

int main(int argc,char** argv){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    if(argc!=2){
        cerr<<"Usage: "<<argv[0]<<" <base>\n";
        return 1;
    }
    string base=argv[1];
    string input=base+".city";
    string satout=base+".satoutput";
    string metromap=base+".metromap";

    ifstream fin(input);
    int scenario; fin>>scenario;

        int N,M,K,J , P=0; 
        fin>>N>>M>>K>>J;
        if (scenario==2) fin>>P;
        vector<pair<int,int>> start_points(K);
        vector<pair<int, int>> end_points(K);
        for(int i=0;i<K;i++){ fin>>start_points[i].first>>start_points[i].second>>end_points[i].first>>end_points[i].second; }
        vector<pair<int,int>> popular_points(P);
        if (scenario==2) { for(int i=0;i<P;i++){ fin>>popular_points[i].first>>popular_points[i].second; } }
    
    fin.close();

    if(scenario==1){ 
        // ofstream fout(metromap); fout<<"0\n"; return 0;
         ifstream fs(satout);
    string line;
     bool SAT=false;
    while(fs>>line){
        if(line=="SAT"){ SAT=true; break; }
        if(line=="UNSAT"){  break; }
    }
    if(!SAT){
        ofstream fout(metromap); 
        fout<<"0\n"; 
        return 0;
    }

    long long XMAX=1LL*N*M*K;
    vector<char> trueVar((size_t)XMAX+1, 0);
    long long lit;
    while(fs>>lit){
        if(lit==0) break;
        if(lit>0 && lit<=XMAX) trueVar[(size_t)lit]=1;
    }
    fs.close();

    auto in_bounds = [&](int x,int y){ return 0<=x && x<N && 0<=y && y<M; };
    const int dx[4]={1,-1,0,0};
    const int dy[4]={0,0,1,-1};
    const char move_dir[4]={'R','L','D','U'};


    vector<vector<vector<char>>> occ(K, vector<vector<char>>(M, vector<char>(N,0)));
    for(int k=0;k<K;k++){
        for(int y=0;y<M;y++){
            for(int x=0;x<N;x++){
                int v=Xvar(k,x,y,N,M);
                if(v>0 && v<=XMAX && trueVar[(size_t)v]) occ[k][y][x]=1;
            }
        }
    }

    vector<string> lines; lines.reserve(K);
    for(int k=0;k<K;k++){
        // quick degree sanity: if start/end not occupied => UNSAT wrt decoder assumptions
        if(!occ[k][start_points[k].second][start_points[k].first] || !occ[k][end_points[k].second][end_points[k].first]){
            ofstream fout(metromap); fout<<"0\n"; return 0;
        }
        // auto deg = [&](int x,int y){
        //     int d=0;
        //     for(int t=0;t<4;t++){
        //         int nx=x+dx[t], ny=y+dy[t];
        //         if(in_bounds(nx,ny) && occ[k][ny][nx]) d++;
        //     }
        //     return d;
        // };

        vector<vector<int>> px(M, vector<int>(N,-1)); 
        vector<vector<int>> py(M, vector<int>(N,-1));
        vector<vector<int>> pdir(M, vector<int>(N,-1));
        queue<pair<int,int>> q;
        q.push(start_points[k]); 
        px[start_points[k].second][start_points[k].first]=start_points[k].first; 
        py[start_points[k].second][start_points[k].first]=start_points[k].second;
        while(!q.empty()){
            auto [x,y]=q.front(); q.pop();
            if(x==end_points[k].first && y==end_points[k].second) break;
            for(int t=0;t<4;t++){
                int nx=x+dx[t], ny=y+dy[t];
                if(!in_bounds(nx,ny) || !occ[k][ny][nx]) continue;
                if(px[ny][nx]!=-1) continue;
                px[ny][nx]=x; py[ny][nx]=y; pdir[ny][nx]=t;
                q.push({nx,ny});
            }
        }
        if(px[end_points[k].second][end_points[k].first]==-1){
            ofstream fout(metromap); fout<<"0\n"; return 0;
        }

        vector<pair<int,int>> path;
        for(int cx=end_points[k].first, cy=end_points[k].second; !(cx==start_points[k].first && cy==start_points[k].second); ){
            path.push_back({cx,cy});
            int nx=px[cy][cx], ny=py[cy][cx];
            cx=nx; cy=ny;
        }
        path.push_back(start_points[k]);
        reverse(path.begin(), path.end());

        // if(deg(start_points[k].first,start_points[k].second)!=1 || deg(end_points[k].first,end_points[k].second)!=1){
        //     ofstream fout(metromap); fout<<"0\n"; return 0;
        // }
        // for(size_t t=1;t+1<path.size();t++){
        //     auto [x,y]=path[t];
        //     if(deg(x,y)!=2){
        //         ofstream fout(metromap); fout<<"0\n"; return 0;
        //     }
        // }

        // Convert path to moves
        string moves;
        for(size_t t=1;t<path.size();t++){
            int x0=path[t-1].first, y0=path[t-1].second;
            int x1=path[t].first,   y1=path[t].second;
            if(x1==x0+1 && y1==y0) moves.push_back('R');
            else if(x1==x0-1 && y1==y0) moves.push_back('L');
            else if(x1==x0 && y1==y0+1) moves.push_back('D');
            else if(x1==x0 && y1==y0-1) moves.push_back('U');
            else { ofstream fout(metromap); fout<<"0\n"; return 0; }
            moves.push_back(' ');
        }
        moves.push_back('0');
        lines.push_back(moves);
    }

    // Write metromap
    ofstream fout(metromap);
    if(!fout){ cerr<<"Cannot write "<<metromap<<"\n"; return 1; }
    for(auto &s: lines) fout<<s<<"\n";
    fout.close();
    }
   
    return 0;
}
