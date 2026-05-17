// Code-2 (Parallel Bubble Sort and Merge Sort)

/*
 * THIS CODE HAS BEEN TESTED AND IS FULLY OPERATIONAL.
 *
 * Problem Statement:
 *  Write a program to implement Parallel Bubble Sort and Merge sort using OpenMP.
 *  Use existing algorithms and measure the performance of sequential and parallel algorithms.
 *
 * Code from HighPerformanceComputing (SPPU - Final Year - Computer Engineering - Content)
 * repository on KSKA Git: https://git.kska.io/sppu-be-comp-content/HighPerformanceComputing
 **/

/*
 * EXECUTION INSTRUCTIONS (Debian-based distributions):
 *
 * i) Install g++ with OpenMP support:
 *   sudo apt update
 *   sudo apt install g++
 *
 * ii) Compile:
 *   g++ -fopenmp Code-2.cpp -o Code-2
 *
 * iii) Execute:
 *   ./Code-2
 **/

// BEGINNING OF CODE
#include <iostream>
#include <vector>
#include <cstdlib>
#include <omp.h>

using namespace std;

void printArray(const vector<int>& arr) {
    for (int num : arr)
        cout << num << " ";
    cout << endl;
}

// Bubble Sort

// Sequential bubble sort.
// Sorts the array using bubble sort by repeatedly swapping adjacent elements.
void sequentialBubbleSort(vector<int>& arr) {
    int n = arr.size();
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1])
                swap(arr[j], arr[j + 1]);
        }
    }
}

// Parallel bubble sort using odd-even transposition.
// Standard bubble sort cannot be parallelized directly: thread on index j
// and thread on index j+1 would both touch arr[j+1] simultaneously (data race).
// Odd-even transposition alternates between two phases each pass:
//   Phase 0 (even): compare pairs (0,1), (2,3), (4,5), ...
//   Phase 1 (odd):  compare pairs (1,2), (3,4), (5,6), ...
// Within each phase every pair is independent, so threads never share elements.
void parallelBubbleSort(vector<int>& arr) {
    int n = arr.size();
    for (int i = 0; i < n; i++) {
        // i % 2 selects even phase (0) or odd phase (1).
        // The starting index of the first pair in each phase matches i % 2.
        #pragma omp parallel for
        for (int j = i % 2; j < n - 1; j += 2) {
            if (arr[j] > arr[j + 1])
                swap(arr[j], arr[j + 1]);
        }
    }
}

// Merge Sort

// Merges two sorted halves arr[left..mid] and arr[mid+1..right] in place.
void merge(vector<int>& arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    vector<int> L(n1), R(n2);
    for (int i = 0; i < n1; i++) L[i] = arr[left + i];
    for (int i = 0; i < n2; i++) R[i] = arr[mid + 1 + i];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2)
        arr[k++] = (L[i] <= R[j]) ? L[i++] : R[j++];

    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
}

void sequentialMergeSort(vector<int>& arr, int left, int right) {
    if (left >= right) return;
    int mid = left + (right - left) / 2;
    sequentialMergeSort(arr, left, mid);
    sequentialMergeSort(arr, mid + 1, right);
    merge(arr, left, mid, right);
}

// Parallel merge sort using OpenMP tasks.
// "#pragma omp parallel sections" inside a recursive function would spawn a
// new thread team at every level of recursion, hundreds of thousands of teams
// for a large array, causing enormous overhead and likely a crash.
// Tasks are lighter: the runtime schedules them across an existing thread pool.
// The depth cutoff switches to sequential below a threshold to avoid spawning
// tasks so small that the overhead exceeds the work itself.
void parallelMergeSortHelper(vector<int>& arr, int left, int right, int depth) {
    if (left >= right) return;
    int mid = left + (right - left) / 2;

    if (depth <= 0) {
        // Below the cutoff the subarray is small enough that sequential is faster.
        sequentialMergeSort(arr, left, mid);
        sequentialMergeSort(arr, mid + 1, right);
    } else {
        #pragma omp task
        parallelMergeSortHelper(arr, left, mid, depth - 1);

        #pragma omp task
        parallelMergeSortHelper(arr, mid + 1, right, depth - 1);

        // Wait for both tasks to finish before merging.
        #pragma omp taskwait
    }

    merge(arr, left, mid, right);
}

void parallelMergeSort(vector<int>& arr, int left, int right) {
    // The single directive creates one thread team for the entire sort.
    // All recursive tasks share this pool instead of creating new teams.
    #pragma omp parallel
    {
        // single ensures only one thread kicks off the root task;
        // the rest wait and pick up the child tasks as they are created.
        #pragma omp single
        parallelMergeSortHelper(arr, left, right, 4); // depth 4 → up to 16 parallel tasks
    }
}

// Main function

int main() {
    int n = 10000; // Adjust this to specify the number of elements.
    vector<int> arr(n);

    for (int i = 0; i < n; i++)
        arr[i] = rand() % 10000;

    double start, end;
    double time_seq_bubble, time_par_bubble;
    double time_seq_merge, time_par_merge;

    // --- Sequential Bubble Sort ---
    vector<int> seqArr = arr;
    start = omp_get_wtime();
    sequentialBubbleSort(seqArr);
    end = omp_get_wtime();
    time_seq_bubble = end - start;
    cout << "Sequential Bubble Sort time: " << time_seq_bubble << " seconds" << endl;

    // --- Parallel Bubble Sort ---
    vector<int> parArr = arr;
    start = omp_get_wtime();
    parallelBubbleSort(parArr);
    end = omp_get_wtime();
    time_par_bubble = end - start;
    cout << "Parallel Bubble Sort time: " << time_par_bubble << " seconds" << endl;

    cout << "Bubble Sort Speedup (Sequential / Parallel) = " << (time_seq_bubble / time_par_bubble) << "x" << endl;

    // --- Sequential Merge Sort ---
    seqArr = arr;
    start = omp_get_wtime();
    sequentialMergeSort(seqArr, 0, n - 1);
    end = omp_get_wtime();
    time_seq_merge = end - start;
    cout << "\nSequential Merge Sort time: " << time_seq_merge << " seconds" << endl;

    // --- Parallel Merge Sort ---
    parArr = arr;
    start = omp_get_wtime();
    parallelMergeSort(parArr, 0, n - 1);
    end = omp_get_wtime();
    time_par_merge = end - start;
    cout << "Parallel Merge Sort time: " << time_par_merge << " seconds" << endl;

    cout << "Merge Sort Speedup (Sequential / Parallel) = " << (time_seq_merge / time_par_merge) << "x" << endl;

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