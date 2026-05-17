#include <iostream>
#include <queue>
#include <stack>
#include <omp.h>
#include <vector>

using namespace std;

class Graph{

    int v;
    vector<vector<int>> adj;

    public:
    Graph(int v){
        this->v = v;
        adj.resize(v);
    }

    void addEdge(int a, int b){
        adj[a].push_back(b);
        adj[b].push_back(a);
    }

    void parallelBfs(int start){
        queue<int> q;
        vector<bool> visited(v,false);

        visited[start] = true;
        q.push(start);

        cout<< "parallel Bfs using openmp"<<endl;

        while(!q.empty()){
            int node = q.front();
            q.pop();
            cout << node <<"->";

            #pragma omp parallel for
            for(int i=0;i<adj[node].size();i++){
                int neighbours = adj[node][i];

                if(!visited[neighbours]){
                    #pragma omp critical
                    if(!visited[neighbours]){
                        visited[neighbours] = true;
                        q.push(neighbours);
                    }
                }
            }
        }
        cout <<endl;
    }

    void parallelDfs(int start){
        vector<bool> visited(v,false);
        stack<int> st;

        visited[start] = true;
        st.push(start);

        cout<< "parallel dfs using openmp"<<endl;

        while(!st.empty()){
            int node = st.top();
            st.pop();
            cout<<node<<"->";

            #pragma omp parallel for
            for(int i=0;i<adj[node].size();i++){
                int neighbour = adj[node][i];

                if(!visited[neighbour]){
                    #pragma omp critical
                    if(!visited[neighbour]){
                        visited[neighbour] = true;
                        st.push(neighbour);
                    }
                }
            }
        }
    }

};

int main(){
    Graph g(6);

    g.addEdge(0,1);
    g.addEdge(0,2);
    g.addEdge(1,3);
    g.addEdge(2,4);
    g.addEdge(3,5);
    g.addEdge(4,5);

    g.parallelBfs(0);

    g.parallelDfs(0);
}