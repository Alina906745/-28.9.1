#include <iostream>
#include <thread>
#include <future>
#include <mutex>
#include <chrono>
#include <random>

using namespace std;

unsigned int numberOfConcurrentThreads;
mutex mtx;


void merge(int array[], int const left, int const mid, int const right)
{
    int const subArrayOne = mid - left + 1;
    int const subArrayTwo = right - mid;

    auto* leftArray = new int[subArrayOne],
        * rightArray = new int[subArrayTwo];

    
    for (auto i = 0; i < subArrayOne; i++)
        leftArray[i] = array[left + i];
    for (auto j = 0; j < subArrayTwo; j++)
        rightArray[j] = array[mid + 1 + j];

    auto indexOfSubArrayOne = 0, indexOfSubArrayTwo = 0;
    int indexOfMergedArray = left;

    
    while (indexOfSubArrayOne < subArrayOne && indexOfSubArrayTwo < subArrayTwo)
    {
        if (leftArray[indexOfSubArrayOne] <= rightArray[indexOfSubArrayTwo])
        {
            array[indexOfMergedArray] = leftArray[indexOfSubArrayOne];
            indexOfSubArrayOne++;
        }
        else
        {
            array[indexOfMergedArray] = rightArray[indexOfSubArrayTwo];
            indexOfSubArrayTwo++;
        }
        indexOfMergedArray++;
    }

    
    while (indexOfSubArrayOne < subArrayOne)
    {
        array[indexOfMergedArray] = leftArray[indexOfSubArrayOne];
        indexOfSubArrayOne++;
        indexOfMergedArray++;
    }

    
    while (indexOfSubArrayTwo < subArrayTwo)
    {
        array[indexOfMergedArray] = rightArray[indexOfSubArrayTwo];
        indexOfSubArrayTwo++;
        indexOfMergedArray++;
    }
    delete[] leftArray;
    delete[] rightArray;
}


void mergeSort(int array[], int const begin, int const end)
{
    if (begin >= end)
        return;

    int mid = begin + (end - begin) / 2;

    mtx.lock();
    if (numberOfConcurrentThreads)
    {
        numberOfConcurrentThreads--;
        mtx.unlock();
        future<void> fut = async(launch::async, [&]() { mergeSort(array, begin, mid); });
        mergeSort(array, mid + 1, end);
        fut.get();
    }
    else
    {
        mtx.unlock();
        mergeSort(array, begin, mid);
        mergeSort(array, mid + 1, end);
    }
    merge(array, begin, mid, end);
    return;
}

void mergeSortWithoutThreads(int array[], int const begin, int const end)
{
    if (begin >= end)
        return;

    int mid = begin + (end - begin) / 2;
    mergeSort(array, begin, mid);
    mergeSort(array, mid + 1, end);
    merge(array, begin, mid, end);
    return;
}


void printArray(int A[], int size)
{
    for (int i = 0; i < size; i++)
        cout << A[i] << " ";
    cout << endl;
}


int main()
{
    numberOfConcurrentThreads = std::thread::hardware_concurrency();
    std::cout << numberOfConcurrentThreads << " concurrent threads are supported.\n";
    numberOfConcurrentThreads--;

    srand(0);
    long arr_size = 1000000;
    int* arr = new int[arr_size];
    for (long i = 0; i < arr_size; i++)
    {
        arr[i] = rand() % 500000;
    }

    

    time_t start, end;

    time(&start);
    mergeSort(arr, 0, arr_size - 1);
    time(&end);

    int seconds = difftime(end, start);
    printf("Threads. The time: %d seconds\n", seconds);

    time(&start);
    mergeSortWithoutThreads(arr, 0, arr_size - 1);
    time(&end);

    seconds = difftime(end, start);
    printf("No threads. The time: %d seconds\n", seconds);

    
}
