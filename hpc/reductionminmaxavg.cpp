#include <iostream>
#include <omp.h>

using namespace std;

int main() {
    int arr[5] = {10, 20, 5, 40, 15};
    int sum = 0;
    int minValue = arr[0];
    int maxValue = arr[0];
    int n = 5;
    // Parallel Sum
    #pragma omp parallel for reduction(+:sum)
    for(int i = 0; i < n; i++) {
        // Shows which thread is processing which element
        cout << "SUM -> Thread "
             << omp_get_thread_num()
             << " processing arr[" << i << "] = "
             << arr[i] << endl;

        sum += arr[i];
    }
    // Parallel Minimum
    #pragma omp parallel for reduction(min:minValue)
    for(int i = 0; i < n; i++) {

        cout << "MIN -> Thread "
             << omp_get_thread_num()
             << " processing arr[" << i << "] = "
             << arr[i] << endl;

        if(arr[i] < minValue) {
            minValue = arr[i];
        }
    }
    // Parallel Maximum
    #pragma omp parallel for reduction(max:maxValue)
    for(int i = 0; i < n; i++) {
        cout << "MAX -> Thread "
             << omp_get_thread_num()
             << " processing arr[" << i << "] = "
             << arr[i] << endl;

        if(arr[i] > maxValue) {
            maxValue = arr[i];
        }
    }
    // Average
    double average = (double)sum / n;

    // Output
    cout << "\nArray Elements: ";
    for(int i = 0; i < n; i++) {
        cout << arr[i] << " ";
    }
    cout << "\n\nSum = " << sum;
    cout << "\nMinimum = " << minValue;
    cout << "\nMaximum = " << maxValue;
    cout << "\nAverage = " << average;
    return 0;
}

/*
Your program correctly implements **Parallel Reduction Operations using OpenMP** for:

* Sum
* Minimum
* Maximum
* Average

This matches the problem statement:

> “Implement Min, Max, Sum and Average operations using Parallel Reduction.”

---

# Theory / Viva Explanation

## What is OpenMP?

OpenMP is an API used for **parallel programming** on shared-memory systems.

It allows multiple threads to execute parts of a program simultaneously using compiler directives like:

```cpp
#pragma omp
```

---

# What is Parallel Computing?

Parallel computing means:

> Breaking a task into smaller parts and executing them simultaneously using multiple processors/threads.

Example:

Instead of one CPU adding all array elements one by one:

```text
10 + 20 + 5 + 40 + 15
```

multiple threads divide the work:

```text
Thread 0 → 10 + 20
Thread 1 → 5 + 40
Thread 2 → 15
```

Then results are combined.

This improves performance for large data.

---

# What is Reduction in OpenMP?

Reduction is an OpenMP technique used when:

> Multiple threads work on a common variable safely.

Without reduction:

```cpp
sum += arr[i];
```

causes **race condition** because many threads modify `sum` simultaneously.

Reduction creates:

* private copy for each thread
* combines results at end

Syntax:

```cpp
reduction(operator:variable)
```

Example:

```cpp
reduction(+:sum)
```

means:

* each thread keeps local `sum`
* OpenMP adds them together at end

---

# Types of Reduction Used

| Reduction | Meaning       |
| --------- | ------------- |
| `+`       | Addition      |
| `min`     | Minimum value |
| `max`     | Maximum value |

---

# Header Files Used

## iostream

```cpp
#include <iostream>
```

Used for input/output operations.

Functions:

* `cout`
* `cin`

---

## omp.h

```cpp
#include <omp.h>
```

Required for OpenMP functions.

Used for:

* thread management
* timing
* thread ID

Example:

```cpp
omp_get_thread_num()
```

returns current thread number.

---

# Important OpenMP Directive

## `#pragma omp parallel for`

This directive tells compiler:

> Execute loop iterations in parallel using multiple threads.

Example:

```cpp
#pragma omp parallel for
for(int i=0;i<n;i++)
```

Different threads execute different iterations.

---

# Race Condition

A race condition occurs when:

> Multiple threads access and modify same variable simultaneously causing unpredictable results.

Example:

```cpp
sum += arr[i];
```

Without reduction:

* Thread 0 updates sum
* Thread 1 updates sum same time
* one update may be lost

Reduction solves this problem.

---

# Sequential vs Parallel Execution

| Sequential            | Parallel                 |
| --------------------- | ------------------------ |
| One thread            | Multiple threads         |
| Slower for large data | Faster                   |
| Simple                | Requires synchronization |
| No race conditions    | Race conditions possible |

---

# Code Explanation

---

## Array Initialization

```cpp
int arr[5] = {10, 20, 5, 40, 15};
```

Stores array elements.

---

## Sum Variables

```cpp
int sum = 0;
int minValue = arr[0];
int maxValue = arr[0];
```

Initial values for reduction operations.

---

# Parallel Sum

```cpp
#pragma omp parallel for reduction(+:sum)
```

Creates parallel threads.

Each thread computes partial sum.

At end:

```text
Final Sum = Sum of all thread sums
```

---

## Thread Display

```cpp
omp_get_thread_num()
```

Returns current thread ID.

Useful for visualization/debugging.

Example output:

```text
SUM -> Thread 1 processing arr[2] = 5
```

---

# Parallel Minimum

```cpp
reduction(min:minValue)
```

Each thread finds local minimum.

OpenMP combines all local minimums.

---

# Parallel Maximum

```cpp
reduction(max:maxValue)
```

Each thread finds local maximum.

OpenMP combines all local maximums.

---

# Average Calculation

```cpp
double average = (double)sum / n;
```

Formula:

\text{Average} = \frac{\text{Sum}}{n}

Type casting:

```cpp
(double)
```

avoids integer division.

---

# Time Complexity

| Operation | Complexity     |
| --------- | -------------- |
| Sum       | O(n)           |
| Min       | O(n)           |
| Max       | O(n)           |
| Average   | O(1) after sum |

Parallel execution reduces effective execution time.

---

# Advantages of OpenMP

* Easy parallel programming
* Simple directives
* Faster execution
* Shared memory support
* Minimal code changes

---

# Limitations

* Works mainly on shared-memory systems
* Thread creation overhead
* Race conditions possible
* Not efficient for very small tasks

---

# Expected Output

```text
Array Elements: 10 20 5 40 15

Sum = 90
Minimum = 5
Maximum = 40
Average = 18
```

Thread order may change every run because scheduling is parallel.

---

# Viva Questions & Answers

## What is OpenMP?

OpenMP is an API for shared-memory parallel programming in C, C++, and Fortran.

---

## What is reduction in OpenMP?

Reduction combines results from multiple threads into a single variable safely.

---

## Why is reduction needed?

To avoid race conditions while updating shared variables.

---

## What is race condition?

When multiple threads modify same data simultaneously causing incorrect results.

---

## Difference between parallel and sequential execution?

Sequential uses one thread; parallel uses multiple threads simultaneously.

---

## What does `omp_get_thread_num()` do?

Returns current executing thread ID.

---

## Why use `(double)sum`?

To perform floating-point division instead of integer division.

*/