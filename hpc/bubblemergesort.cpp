#include<iostream>      // Input Output
#include<omp.h>         // OpenMP
#include<chrono>        // High precision timing
#include<iomanip>       // Decimal precision

using namespace std;
using namespace chrono;


// --------------------------------------------------
// SEQUENTIAL BUBBLE SORT
// --------------------------------------------------

void bubbleSortSequential(int arr[], int n){

    for(int i = 0; i < n-1; i++){

        for(int j = 0; j < n-i-1; j++){

            // Swap if wrong order
            if(arr[j] > arr[j+1]){

                swap(arr[j], arr[j+1]);
            }
        }
    }
}


// --------------------------------------------------
// PARALLEL BUBBLE SORT
// --------------------------------------------------

void bubbleSortParallel(int arr[], int n){

    for(int i = 0; i < n-1; i++){

        // Parallel loop
        #pragma omp parallel for
        for(int j = 0; j < n-i-1; j++){

            // Swap elements
            if(arr[j] > arr[j+1]){

                swap(arr[j], arr[j+1]);
            }
        }
    }
}


// --------------------------------------------------
// MERGE FUNCTION
// --------------------------------------------------

void merge(int arr[], int low, int mid, int high){

    int temp[100];

    int i = low;
    int j = mid + 1;
    int k = low;

    // Compare elements
    while(i <= mid && j <= high){

        if(arr[i] < arr[j]){

            temp[k] = arr[i];
            i++;
        }
        else{

            temp[k] = arr[j];
            j++;
        }

        k++;
    }

    // Remaining left elements
    while(i <= mid){

        temp[k] = arr[i];
        i++;
        k++;
    }

    // Remaining right elements
    while(j <= high){

        temp[k] = arr[j];
        j++;
        k++;
    }

    // Copy temp array back
    for(i = low; i <= high; i++){

        arr[i] = temp[i];
    }
}


// --------------------------------------------------
// SEQUENTIAL MERGE SORT
// --------------------------------------------------

void mergeSortSequential(int arr[], int low, int high){

    if(low < high){

        int mid = (low + high) / 2;

        // Left half
        mergeSortSequential(arr, low, mid);

        // Right half
        mergeSortSequential(arr, mid + 1, high);

        // Merge
        merge(arr, low, mid, high);
    }
}


// --------------------------------------------------
// PARALLEL MERGE SORT
// --------------------------------------------------

void mergeSortParallel(int arr[], int low, int high){

    if(low < high){

        int mid = (low + high) / 2;

        // Parallel sections
        #pragma omp parallel sections
        {

            // Left section
            #pragma omp section
            mergeSortParallel(arr, low, mid);

            // Right section
            #pragma omp section
            mergeSortParallel(arr, mid + 1, high);
        }

        // Merge both halves
        merge(arr, low, mid, high);
    }
}


// --------------------------------------------------
// PRINT ARRAY
// --------------------------------------------------

void printArray(int arr[], int n){

    for(int i = 0; i < n; i++){

        cout << arr[i] << " ";
    }
}


// --------------------------------------------------
// MAIN FUNCTION
// --------------------------------------------------

int main(){

    // Bubble Sort Arrays
    int bubbleSeq[] = {5,2,9,1,6};
    int bubblePar[] = {5,2,9,1,6};

    // Merge Sort Arrays
    int mergeSeq[] = {8,4,2,6,1};
    int mergePar[] = {8,4,2,6,1};

    int n = 5;


    // --------------------------------------------------
    // SEQUENTIAL BUBBLE SORT TIMING
    // --------------------------------------------------

    auto start1 = high_resolution_clock::now();

    bubbleSortSequential(bubbleSeq, n);

    auto stop1 = high_resolution_clock::now();

    auto duration1 = duration_cast<microseconds>(stop1 - start1);


    // --------------------------------------------------
    // PARALLEL BUBBLE SORT TIMING
    // --------------------------------------------------

    auto start2 = high_resolution_clock::now();

    bubbleSortParallel(bubblePar, n);

    auto stop2 = high_resolution_clock::now();

    auto duration2 = duration_cast<microseconds>(stop2 - start2);


    // --------------------------------------------------
    // SEQUENTIAL MERGE SORT TIMING
    // --------------------------------------------------

    auto start3 = high_resolution_clock::now();

    mergeSortSequential(mergeSeq, 0, n-1);

    auto stop3 = high_resolution_clock::now();

    auto duration3 = duration_cast<microseconds>(stop3 - start3);


    // --------------------------------------------------
    // PARALLEL MERGE SORT TIMING
    // --------------------------------------------------

    auto start4 = high_resolution_clock::now();

    mergeSortParallel(mergePar, 0, n-1);

    auto stop4 = high_resolution_clock::now();

    auto duration4 = duration_cast<microseconds>(stop4 - start4);


    // --------------------------------------------------
    // OUTPUT
    // --------------------------------------------------

    cout << "\nSequential Bubble Sort: ";
    printArray(bubbleSeq, n);

    cout << "\nTime = " << duration1.count() << " microseconds\n";


    cout << "\nParallel Bubble Sort: ";
    printArray(bubblePar, n);

    cout << "\nTime = " << duration2.count() << " microseconds\n";


    cout << "\nSequential Merge Sort: ";
    printArray(mergeSeq, n);

    cout << "\nTime = " << duration3.count() << " microseconds\n";


    cout << "\nParallel Merge Sort: ";
    printArray(mergePar, n);

    cout << "\nTime = " << duration4.count() << " microseconds\n";


    return 0;
}/*
Your code implements:

# Parallel Bubble Sort and Parallel Merge Sort using OpenMP

This satisfies the practical statement:

> “Write a program to implement Parallel Bubble Sort and Merge Sort using OpenMP. Use existing algorithms and measure performance of sequential and parallel algorithms.” 

---

# Theory / Viva Explanation

---

# What is Sorting?

Sorting means:

> Arranging data in a particular order.

Usually:

* Ascending order
* Descending order

Example:

```text id="98vdv7"
5 2 9 1 6
```

After sorting:

```text id="jlwmvn"
1 2 5 6 9
```

---

# Types of Sorting Used

| Algorithm   | Type               |
| ----------- | ------------------ |
| Bubble Sort | Comparison-based   |
| Merge Sort  | Divide and Conquer |

---

# What is Sequential Sorting?

Sequential sorting means:

> One thread performs all operations step-by-step.

Only one CPU core used.

---

# What is Parallel Sorting?

Parallel sorting means:

> Multiple threads perform sorting simultaneously.

Advantages:

* Faster execution
* Better CPU utilization
* Useful for large datasets

Implemented using OpenMP.

---

# Header Files Used

---

## iostream

```cpp id="h5ql4z"
#include<iostream>
```

Used for input/output operations.

---

## omp.h

```cpp id="54z6ak"
#include<omp.h>
```

Provides OpenMP functions and directives.

---

## chrono

```cpp id="r9rm2f"
#include<chrono>
```

Used for measuring execution time.

---

## iomanip

```cpp id="zjod0g"
#include<iomanip>
```

Used for formatting output precision.

---

# What is OpenMP?

OpenMP is an API for:

> Shared-memory parallel programming in C/C++.

Uses compiler directives like:

```cpp id="u25kmh"
#pragma omp
```

to create threads automatically.

---

# Bubble Sort Theory

## Definition

Bubble Sort repeatedly compares adjacent elements and swaps them if they are in wrong order.

Largest element “bubbles” to the end after each pass.

---

# Bubble Sort Working

Example:

```text id="2qj7p9"
5 2 9 1 6
```

Pass 1:

```text id="zt5f48"
2 5 1 6 9
```

Pass 2:

```text id="zbvfpc"
2 1 5 6 9
```

Pass 3:

```text id="ojpk1f"
1 2 5 6 9
```

---

# Bubble Sort Formula

\text{Bubble Sort repeatedly swaps adjacent elements if they are in wrong order}

---

# Sequential Bubble Sort

```cpp id="jlwmni"
void bubbleSortSequential(int arr[], int n)
```

Uses nested loops.

Outer loop:

```cpp id="3aw7od"
for(int i = 0; i < n-1; i++)
```

controls passes.

Inner loop:

```cpp id="77gjfy"
for(int j = 0; j < n-i-1; j++)
```

compares adjacent elements.

---

# Swapping

```cpp id="72vd90"
swap(arr[j], arr[j+1]);
```

Exchanges elements.

---

# Parallel Bubble Sort

```cpp id="sh7d0k"
#pragma omp parallel for
```

Parallelizes comparison loop.

Multiple threads compare different array elements simultaneously.

---

# Limitation of Parallel Bubble Sort

Bubble sort is not ideal for parallelization because:

* Adjacent swaps depend on previous swaps
* Synchronization overhead occurs

Still used for educational understanding of OpenMP.

---

# Merge Sort Theory

## Definition

Merge Sort uses:

> Divide and Conquer technique

Steps:

1. Divide array into halves
2. Sort each half
3. Merge sorted halves

---

# Merge Sort Representation

\text{Merge Sort: Divide \rightarrow Sort \rightarrow Merge}

---

# Merge Function

```cpp id="80js8t"
void merge(int arr[], int low, int mid, int high)
```

Combines two sorted halves into one sorted array.

---

# Merge Working

Example:

Left:

```text id="qq0xko"
1 4 8
```

Right:

```text id="qyn4lj"
2 6
```

Merged:

```text id="1kwp4h"
1 2 4 6 8
```

---

# Sequential Merge Sort

```cpp id="ek7my2"
mergeSortSequential(arr, low, high);
```

Recursively divides array.

---

# Recursion

Function calling itself repeatedly.

Base condition:

```cpp id="63xtnd"
if(low < high)
```

stops recursion.

---

# Mid Calculation

```cpp id="d0jsn7"
int mid = (low + high) / 2;
```

Splits array into halves.

---

# Parallel Merge Sort

Uses:

```cpp id="6knzh1"
#pragma omp parallel sections
```

Creates parallel tasks for:

* left half
* right half

---

# OpenMP Sections

```cpp id="sjtt8n"
#pragma omp section
```

Each section executed by separate thread.

---

# Why Merge Sort is Better for Parallelism?

Because:

* left half independent
* right half independent

Perfect for concurrent execution.

---

# Time Measurement

Uses:

```cpp id="g2dlko"
high_resolution_clock::now()
```

to capture execution start and end time.

---

# Duration Calculation

```cpp id="fqxwom"
duration_cast<microseconds>
```

Measures execution time in microseconds.

---

# Time Complexity

| Algorithm   | Best       | Average    | Worst      |
| ----------- | ---------- | ---------- | ---------- |
| Bubble Sort | O(n)       | O(n²)      | O(n²)      |
| Merge Sort  | O(n log n) | O(n log n) | O(n log n) |

---

# Merge Sort Complexity Formula

T(n)=2T\left(\frac{n}{2}\right)+O(n)

---

# Space Complexity

| Algorithm   | Space |
| ----------- | ----- |
| Bubble Sort | O(1)  |
| Merge Sort  | O(n)  |

Merge sort needs temporary array.

---

# Advantages of Parallel Sorting

* Faster for large datasets
* Efficient CPU utilization
* Reduces execution time
* Scalable

---

# Limitations

* Thread creation overhead
* Synchronization cost
* Small arrays may not show improvement
* Parallel Bubble Sort inefficient compared to Merge Sort

---

# Output Explanation

Example:

```text id="69v9s5"
Sequential Bubble Sort: 1 2 5 6 9
Time = 5 microseconds
```

Shows:

* sorted array
* execution time

---

# Important Viva Questions

---

## What is Bubble Sort?

Bubble Sort repeatedly swaps adjacent elements until array becomes sorted.

---

## What is Merge Sort?

Merge Sort divides array recursively and merges sorted halves.

---

## Which sorting is better?

Merge Sort is better because complexity is:

O(n\log n) < O(n^2)

---

## Why is Merge Sort suitable for parallelization?

Because left and right halves can be processed independently.

---

## What is Divide and Conquer?

Technique where problem divided into smaller subproblems and solved recursively.

---

## What is OpenMP?

OpenMP is an API for parallel programming using shared memory.

---

## What is `#pragma omp parallel for`?

It parallelizes loop iterations across multiple threads.

---

## What is `#pragma omp parallel sections`?

Executes independent code sections in parallel.

---

## Why measure execution time?

To compare performance between sequential and parallel algorithms.

---

## What is recursion?

A function calling itself repeatedly until base condition reached.

---

## Why Bubble Sort is not ideal for parallelization?

Because adjacent swaps depend on each other causing synchronization issues.

---

## Which algorithm is more efficient for large data?

Merge Sort due to:

O(n\log n)

complexity.
*/