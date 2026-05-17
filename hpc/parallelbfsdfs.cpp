#include <iostream>
#include <vector>
#include <omp.h>

using namespace std;

// Undirected graph with parallel BFS and DFS traversal via OpenMP.
class Graph {
    int V;
    vector<vector<int>> adj;

public:
    Graph(int V) {
        this->V = V;
        adj.resize(V);
    }

    void addEdge(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    // Level-synchronous BFS: all nodes at the current depth (the "frontier")
    // are expanded in parallel before moving to the next level. This is the
    // natural unit of parallelism for BFS, processing individual nodes is too
    // fine-grained for threads to be useful.
    void parallelBFS(int start) {
        vector<bool> visited(V, false);
        vector<int> frontier;

        visited[start] = true;
        frontier.push_back(start);

        cout << "Parallel BFS from node " << start << ": ";

        while (!frontier.empty()) {
            for (int u : frontier)
                cout << u << " ";

            vector<int> next_frontier;

            // Each thread accumulates its own local candidates to avoid
            // contention on a shared next_frontier vector.
            #pragma omp parallel
            {
                vector<int> local_next;

                // nowait: threads that finish early skip the implicit barrier
                // and proceed directly to the merge below.
                // schedule(dynamic): faster threads pick up remaining chunks
                // when adjacency list sizes vary across nodes.
                #pragma omp for nowait schedule(dynamic)
                for (int i = 0; i < (int)frontier.size(); i++) {
                    for (int v : adj[frontier[i]]) {
                        // The check-and-set on visited[] must be a single
                        // critical section — without it, two threads could
                        // both see visited[v]==false and both enqueue v,
                        // producing duplicates in the next frontier.
                        bool should_visit = false;
                        #pragma omp critical
                        {
                            if (!visited[v]) {
                                visited[v] = true;
                                should_visit = true;
                            }
                        }
                        // local_next is thread-private so no lock needed here.
                        if (should_visit)
                            local_next.push_back(v);
                    }
                }

                // Merge: one thread at a time appends its local results.
                // This is a separate critical section from the one above
                // so the two do not serialize against each other.
                #pragma omp critical
                {
                    next_frontier.insert(next_frontier.end(),
                                         local_next.begin(),
                                         local_next.end());
                }
            } // implicit barrier: all threads finish before frontier is swapped

            frontier = next_frontier;
        }

        cout << endl;
    }

    // Iterative DFS using a vector as a stack (push_back/pop_back).
    // vector is used instead of std::stack because std::stack cannot be
    // safely shared across threads.
    void parallelDFS(int start) {
        vector<bool> visited(V, false);
        vector<int> stack;

        stack.push_back(start);

        cout << "Parallel DFS from node " << start << ": ";

        while (!stack.empty()) {
            int u = stack.back();
            stack.pop_back();

            // A node may be pushed multiple times before it is marked visited
            // (two threads can both see visited[v]==false). This guard ensures
            // it is processed only once.
            if (visited[u]) continue;
            visited[u] = true;
            cout << u << " ";

            vector<int> to_push;

            #pragma omp parallel
            {
                vector<int> local_push;

                #pragma omp for nowait schedule(dynamic)
                for (int i = 0; i < (int)adj[u].size(); i++) {
                    // visited[] is only read here, not written, so no critical
                    // section is needed. Stale reads may cause duplicates but
                    // the guard above handles that safely.
                    if (!visited[adj[u][i]])
                        local_push.push_back(adj[u][i]);
                }

                #pragma omp critical
                {
                    to_push.insert(to_push.end(),
                                   local_push.begin(),
                                   local_push.end());
                }
            }

            for (int v : to_push)
                stack.push_back(v);
        }

        cout << endl;
    }
};

int main() {
    Graph g(6);

    g.addEdge(0, 1);
    g.addEdge(0, 2);
    g.addEdge(1, 3);
    g.addEdge(1, 4);
    g.addEdge(2, 5);

    g.parallelBFS(0);
    g.parallelDFS(0);

    return 0;
}
/*# Theory for Parallel Bubble Sort and Merge Sort using OpenMP

Your program implements:

* Sequential Bubble Sort
* Parallel Bubble Sort
* Sequential Merge Sort
* Parallel Merge Sort

using OpenMP. 

---

# What is Sorting?

Sorting means:

> Arranging data in a particular order.

Usually:

* ascending order
* descending order

Example:

```text id="jlwmt1"
5 2 9 1 6
```

After sorting:

```text id="jlwmt2"
1 2 5 6 9
```

---

# Why Sorting is Important?

Sorting is used in:

* searching algorithms
* databases
* data analysis
* operating systems
* machine learning

---

# What is Sequential Sorting?

Sequential sorting means:

> A single thread performs all operations one after another.

Only one processor/core used.

---

# What is Parallel Sorting?

Parallel sorting means:

> Multiple threads execute sorting operations simultaneously.

Advantages:

* faster execution
* better CPU utilization
* reduced processing time

---

# What is OpenMP?

OpenMP is an API for parallel programming in C/C++.

It uses compiler directives like:

```cpp id="jlwmt3"
#pragma omp parallel for
```

to create multiple threads automatically.

---

# Bubble Sort

## Definition

Bubble Sort repeatedly compares adjacent elements and swaps them if they are in wrong order.

Largest element “bubbles” to correct position after each pass.

---

# Bubble Sort Example

Initial array:

```text id="jlwmt4"
5 2 9 1 6
```

After passes:

```text id="jlwmt5"
2 5 1 6 9
2 1 5 6 9
1 2 5 6 9
```

---

# Sequential Bubble Sort

Your code:

```cpp id="jlwmt6"
void sequentialBubbleSort(vector<int>& arr)
```

uses nested loops to compare adjacent elements sequentially. 

---

# Bubble Sort Time Complexity

| Case    | Complexity |
| ------- | ---------- |
| Best    | O(n)       |
| Average | O(n²)      |
| Worst   | O(n²)      |

---

# Bubble Sort Formula

O(n^2)

---

# Why Normal Bubble Sort Cannot Be Parallelized Directly?

Normal bubble sort has:

> Data dependency

Example:

```text id="jlwmt7"
arr[j] and arr[j+1]
```

and

```text id="jlwmt8"
arr[j+1] and arr[j+2]
```

both modify same element.

This creates:

> Race condition

---

# Race Condition

Race condition occurs when:

> Multiple threads access same memory simultaneously causing incorrect results.

---

# Parallel Bubble Sort

Your code uses:

## Odd-Even Transposition Sort

instead of standard bubble sort. 

---

# Odd-Even Transposition

Sorting occurs in phases.

---

## Even Phase

Compare:

```text id="jlwmt9"
(0,1) (2,3) (4,5)
```

---

## Odd Phase

Compare:

```text id="jlwmta"
(1,2) (3,4) (5,6)
```

Now comparisons become independent.

Safe for parallel execution.

---

# OpenMP Directive Used

```cpp id="jlwmtb"
#pragma omp parallel for
```

Parallelizes loop iterations among multiple threads.

---

# Parallel Bubble Sort Complexity

Practical complexity:

O\left(\frac{n^2}{P}+n\right)

Where:

* ( n ) = number of elements
* ( P ) = processors

---

# Why Parallel Bubble Sort May Be Slower?

Because of:

* thread creation overhead
* synchronization barriers
* data dependency
* small dataset size

---

# Merge Sort

## Definition

Merge Sort uses:

> Divide and Conquer technique

Steps:

1. Divide array into halves
2. Sort halves recursively
3. Merge sorted halves

---

# Merge Sort Representation

Divide \rightarrow Sort \rightarrow Merge

---

# Merge Function

Your merge function combines two sorted halves. 

---

# Sequential Merge Sort

Implemented recursively using:

```cpp id="jlwmtc"
sequentialMergeSort(arr,left,right)
```



---

# Recursion

Recursion means:

> Function calling itself repeatedly.

Base condition:

```cpp id="jlwmtd"
if(left >= right)
```

stops recursion.

---

# Merge Sort Complexity

| Case    | Complexity |
| ------- | ---------- |
| Best    | O(n log n) |
| Average | O(n log n) |
| Worst   | O(n log n) |

---

# Merge Sort Formula

T(n)=2T\left(\frac{n}{2}\right)+O(n)

---

# Why Merge Sort is Good for Parallelism?

Because:

* left half independent
* right half independent

Both can execute simultaneously.

---

# Parallel Merge Sort

Your code uses:

```cpp id="jlwmte"
#pragma omp task
```

to create parallel recursive tasks. 

---

# OpenMP Tasks

Tasks are lightweight parallel jobs scheduled by OpenMP runtime.

Advantages:

* efficient recursion
* avoids repeated thread creation
* dynamic scheduling

---

# Taskwait

```cpp id="jlwmtf"
#pragma omp taskwait
```

waits until child tasks complete before merge step.

---

# Why Not Use `parallel sections` Recursively?

Your code correctly explains this. 

Recursive `parallel sections` would create:

```text id="jlwmtg"
thousands of thread teams
```

causing huge overhead.

Tasks are more efficient.

---

# Depth Cutoff

Your code uses:

```cpp id="jlwmth"
depth <= 0
```

to switch to sequential merge sort for small subarrays. 

Reason:

> Task overhead may exceed useful work for very small arrays.

---

# Performance Measurement

Your code uses:

```cpp id="jlwmti"
omp_get_wtime()
```

for timing. 

---

# Execution Time Formula

Execution\ Time = End\ Time - Start\ Time

---

# Speedup

Measures parallel performance improvement.

Formula:

Speedup = \frac{Sequential\ Time}{Parallel\ Time}

---

# Interpretation

| Speedup | Meaning           |
| ------- | ----------------- |
| > 1     | Parallel faster   |
| < 1     | Sequential faster |

---

# Space Complexity

| Algorithm   | Space |
| ----------- | ----- |
| Bubble Sort | O(1)  |
| Merge Sort  | O(n)  |

Merge sort needs temporary arrays.

---

# Advantages of Parallel Merge Sort

* highly scalable
* efficient parallelism
* lower complexity
* better for large datasets

---

# Limitations

* synchronization overhead
* task management overhead
* memory usage in merge sort

---

# Expected Output

Example:

```text id="jlwmtj"
Sequential Bubble Sort time: 0.95 seconds
Parallel Bubble Sort time: 0.28 seconds

Sequential Merge Sort time: 0.01 seconds
Parallel Merge Sort time: 0.002 seconds
```

---

# Important Viva Questions

---

## What is OpenMP?

OpenMP is an API for shared-memory parallel programming.

---

## Why use parallel sorting?

To improve performance using multiple processors.

---

## Why is Bubble Sort difficult to parallelize?

Because adjacent swaps have data dependency.

---

## What is Odd-Even Transposition?

Parallel version of Bubble Sort using alternating odd/even phases.

---

## Why Merge Sort better for parallelism?

Because subarrays can be processed independently.

---

## What is recursion?

Function calling itself repeatedly.

---

## What is task parallelism?

Breaking work into independent tasks executed simultaneously.

---

## What does `#pragma omp task` do?

Creates parallel tasks in OpenMP.

---

## What is race condition?

Simultaneous access/modification of shared data causing incorrect results.

---

## What is speedup?

Ratio of sequential execution time to parallel execution time.

---

## Why use depth cutoff in parallel merge sort?

To avoid task overhead for very small subarrays.

---

## Difference between sequential and parallel algorithms?

| Sequential            | Parallel                 |
| --------------------- | ------------------------ |
| One thread            | Multiple threads         |
| Slower for large data | Faster for large data    |
| Simpler               | Requires synchronization |
*/