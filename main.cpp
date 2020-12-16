#include <iostream>
#include <random>

#include "HashSet.h"

using namespace std;

void benchTime(const int* arr, const int arrSize, HashSet set){
    size_t summaryNs = 0;
    size_t longestNs = 0;
    for(int i = 0; i < arrSize; i++) {
        int v = arr[i];

        size_t time = set.benchTime_insert(v);
        summaryNs += time;
        if(time > longestNs)
            longestNs = time;
    }
    size_t averageNs = summaryNs / arrSize;
    cout << "average insert time: " << averageNs << " ns, "
    << "worst: " << longestNs << " ns (" << longestNs / averageNs << "x) " << endl;
    set.printDebug();
}

void benchOps(const int* arr, const int arrSize, HashSet set){
    size_t summaryOps = 0;
    size_t longestOps = 0;
    for(int i = 0; i < arrSize; i++) {
        int v = arr[i];

        size_t ops = set.benchOps_insert(v);
        summaryOps += ops;
        if(ops > longestOps)
            longestOps = ops;

    }
    double averageOps = (double) summaryOps / arrSize;
    cout << "average operations to insert: " << averageOps << ", "
         << "worst: " << longestOps << endl;
}

void allocatedVsUnallocated(const int* arr, const int arrSize){
    HashSet allocated = HashSet(arrSize * 2);
    HashSet unallocated;
    cout << "Inserting to unallocated set:" << endl;
    benchTime(arr, arrSize, unallocated);
    cout << "Inserting to allocated set:" << endl;
    benchTime(arr, arrSize, allocated);
}

void compareHashes(const int* arr, const int arrSize){
    HashSet h[] = {
        HashSet(HashSet::set_nothingHash),
        HashSet(HashSet::set_xorshiftHash),
        HashSet(HashSet::set_robertJenkinsHash),
        HashSet(HashSet::set_shift32multHash)
    };
    char* names[] = {
            (char *)"nothingHash",
            (char *)"xorshiftHash",
            (char *)"robertJenkinsHash",
            (char *)"shift32multHash",
    };
    cout << "Inserting " << arrSize << " random elements." << endl;
    for(int i = 0; i < 4; i++){
        cout << "Using: " << names[i] << endl;
        benchTime(arr, arrSize, h[i]);
    }

//    cout << endl;
//    for(int i = 0; i < 4; i++){
//        h[i].makeEmpty();
//    }
//    int* arr2 = new int[arrSize];
//    for(int i = 0; i < arrSize; i++)
//        arr2[i] = i;
//    cout << "Inserting " << arrSize << " elements from 0 to " << arrSize << "." << endl;
//    for(int i = 0; i < 4; i++){
//        cout << "Using: " << names[i] << endl;
//        bench(arr, arrSize, h[i]);
//    }
//    delete[] arr2;
}

bool everythingIsInSet(const int* arr, const int arrSize){
    HashSet fromArr(arr, arrSize);
    for(size_t i = 0; i < arrSize; i++){
        if(!fromArr.contains(arr[i]))
            throw "Not in set!";
    }
    return true;
}

int* createRandomIntArray(size_t size){
    random_device rd;
    mt19937 g(rd());
    int* arr = new int[size];
    for(size_t i = 0; i < size; i++)
        arr[i] = g();
    return arr;
}

void bigBenchmark(){
    int arrSize = 1000000;
    int* arr = createRandomIntArray(arrSize);
    auto t0 = high_resolution_clock::now();
    HashSet big(arr, arrSize);
    auto t1 = high_resolution_clock::now();
    auto dur = duration_cast<milliseconds>(t1 - t0);
    cout << endl << "Inserting million elements to HashSet took only: " << (float) dur.count() / 100 << " sec" << endl;
    delete[] arr;
}

int main() {
    const int arrSize = 100000;
    const int* arr = createRandomIntArray(arrSize);

    compareHashes(arr, arrSize);
    cout << endl;
    allocatedVsUnallocated(arr, arrSize);
    bigBenchmark();
    if(everythingIsInSet(arr, arrSize))
        cout << endl << "Every number added to set can be found" << endl;

    delete[] arr;
    return 0;
}
