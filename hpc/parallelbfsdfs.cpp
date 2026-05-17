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
/*Your program implements:

# Parallel Breadth First Search (BFS) and Depth First Search (DFS) using OpenMP

This satisfies the practical statement:

> “Design and implement Parallel Breadth First Search and Depth First Search based on existing algorithms using OpenMP.”

---

# Theory / Viva Explanation

---

# What is a Graph?

A graph is a non-linear data structure consisting of:

* Vertices (nodes)
* Edges (connections)

Example:

```text id="htnqot"
0 ---- 1
|      |
2      3
 \    /
   4
```

---

# Types of Graphs

| Type             | Description          |
| ---------------- | -------------------- |
| Directed Graph   | Edges have direction |
| Undirected Graph | Edges work both ways |
| Weighted Graph   | Edges have weights   |
| Unweighted Graph | No weights           |

Your program uses:

> Undirected Unweighted Graph

because:

```cpp id="2nqkz4"
adj[a].push_back(b);
adj[b].push_back(a);
```

adds edges both ways.

---

# Graph Representation

## Adjacency List

Your graph uses adjacency list representation:

```cpp id="q87cx4"
vector<vector<int>> adj;
```

Each index stores neighbors of a node.

Example:

```text id="d9moxq"
0 → 1,2
1 → 0,3
2 → 0,4
```

Advantages:

* Memory efficient
* Faster traversal
* Good for sparse graphs

---

# What is BFS?

## Breadth First Search

BFS traverses graph:

> Level by level

It uses:

> Queue (FIFO)

Traversal order example:

```text id="pgrq1x"
0 → 1 → 2 → 3 → 4 → 5
```

Applications:

* Shortest path
* Network routing
* Web crawling
* Social networks

---

# BFS Working

Steps:

1. Start node inserted into queue
2. Visit node
3. Add unvisited neighbors
4. Repeat until queue empty

---

# BFS Formula Representation

\text{BFS explores nodes level by level using a Queue (FIFO)}

---

# What is DFS?

## Depth First Search

DFS traverses graph:

> Depth-wise first

It uses:

> Stack (LIFO)

Traversal example:

```text id="6k8o5g"
0 → 2 → 4 → 5 → 3 → 1
```

Applications:

* Cycle detection
* Path finding
* Topological sorting
* Maze solving

---

# DFS Working

Steps:

1. Push start node into stack
2. Visit node
3. Push unvisited neighbors
4. Continue until stack empty

---

# DFS Representation

\text{DFS explores depth-wise using a Stack (LIFO)}

---

# What is OpenMP?

OpenMP is an API used for parallel programming in C/C++.

It creates multiple threads to execute tasks simultaneously.

---

# Why Parallel BFS/DFS?

Normal BFS/DFS are sequential.

Parallel version:

* Processes neighbors simultaneously
* Uses multiple threads
* Improves speed for large graphs

---

# Important OpenMP Directives Used

---

## `#pragma omp parallel for`

```cpp id="b3b7kl"
#pragma omp parallel for
```

Converts loop into parallel execution.

Different iterations handled by different threads.

---

## `#pragma omp critical`

```cpp id="0jlwm4"
#pragma omp critical
```

Critical section means:

> Only one thread can execute this block at a time.

Used to protect shared resources like:

* queue
* stack
* visited array

---

# Why Critical Section Needed?

Without critical section:

Multiple threads may:

* push same node multiple times
* modify queue simultaneously
* corrupt data

This causes:

> Race Condition

---

# What is Race Condition?

Race condition occurs when:

> Multiple threads access shared data simultaneously causing unpredictable results.

Example:

Two threads trying:

```cpp id="h41szs"
visited[node] = true;
```

at same time.

---

# Class Explanation

---

# Graph Class

```cpp id="9dhrjk"
class Graph
```

Represents graph structure and operations.

Contains:

* number of vertices
* adjacency list
* BFS and DFS functions

---

# Constructor

```cpp id="4jol0o"
Graph(int v)
```

Initializes graph with `v` vertices.

---

# Edge Addition

```cpp id="8ntrtl"
void addEdge(int a, int b)
```

Adds connection between nodes.

Because graph is undirected:

```cpp id="mx9mbm"
adj[a].push_back(b);
adj[b].push_back(a);
```

---

# Parallel BFS Explanation

---

## Queue Declaration

```cpp id="edw4s6"
queue<int> q;
```

Used for BFS traversal.

FIFO structure.

---

## Visited Array

```cpp id="p36bjn"
vector<bool> visited(v,false);
```

Tracks visited nodes.

Prevents infinite loops.

---

## Start Node

```cpp id="xazewq"
visited[start] = true;
q.push(start);
```

Marks source node visited.

---

## BFS Loop

```cpp id="h6r4q5"
while(!q.empty())
```

Runs until all nodes processed.

---

## Remove Front Node

```cpp id="u9ov8l"
int node = q.front();
q.pop();
```

Processes current node.

---

## Parallel Neighbor Processing

```cpp id="jlwmna"
#pragma omp parallel for
```

Processes all neighbors simultaneously.

---

## Critical Section

```cpp id="7q1mkc"
#pragma omp critical
```

Ensures thread-safe queue insertion.

---

# Parallel DFS Explanation

DFS logic is similar except:

Uses:

```cpp id="i5g6k7"
stack<int> st;
```

instead of queue.

LIFO traversal.

---

# Time Complexity

## Sequential

| Algorithm | Complexity |
| --------- | ---------- |
| BFS       | O(V + E)   |
| DFS       | O(V + E)   |

Where:

* V = vertices
* E = edges

---

## Parallel Complexity

Execution time improves because neighbor traversal is parallelized.

Actual speed depends on:

* number of cores
* graph size
* thread overhead

---

# Space Complexity

| Structure      | Complexity |
| -------------- | ---------- |
| Adjacency List | O(V + E)   |
| Queue/Stack    | O(V)       |
| Visited Array  | O(V)       |

---

# Advantages of Parallel BFS/DFS

* Faster traversal
* Efficient for large graphs
* Better CPU utilization
* Useful in AI and networking

---

# Limitations

* Synchronization overhead
* Critical sections reduce parallel efficiency
* Complex debugging
* Small graphs may not benefit much

---

# Expected Output

Possible BFS:

```text id="7kvd3p"
parallel Bfs using openmp
0->1->2->3->4->5->
```

Possible DFS:

```text id="l2n1uy"
parallel dfs using openmp
0->2->4->5->3->1->
```

Order may vary because threads execute in parallel.

---

# Important Viva Questions

---

## What is BFS?

Breadth First Search traverses graph level by level using queue.

---

## What is DFS?

Depth First Search traverses graph depth-wise using stack.

---

## Difference between BFS and DFS?

| BFS                  | DFS                   |
| -------------------- | --------------------- |
| Uses Queue           | Uses Stack            |
| Level-wise traversal | Depth-wise traversal  |
| Finds shortest path  | Good for backtracking |

---

## Why use visited array?

To avoid revisiting nodes and infinite loops.

---

## Why use critical section?

To avoid race conditions while accessing shared resources.

---

## What is adjacency list?

A graph representation where each node stores list of neighbors.

---

## What is OpenMP?

OpenMP is an API for shared-memory parallel programming.

---

## What is race condition?

Simultaneous access/modification of shared data causing incorrect results.

---

## Why parallelize BFS/DFS?

To improve traversal speed using multiple threads.
*/