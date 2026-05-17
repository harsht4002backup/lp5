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