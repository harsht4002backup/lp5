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
}