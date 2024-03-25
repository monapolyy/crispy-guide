#include <time.h>
#include <stdlib.h>
#include <stdio.h>

int extraMemoryAllocated;

void* Alloc(size_t sz)
{
    extraMemoryAllocated += sz;
    size_t* ret = malloc(sizeof(size_t) + sz);
    *ret = sz;
    printf("Extra memory allocated, size: %ld\n", sz);
    return &ret[1];
}

void DeAlloc(void* ptr)
{
    size_t* pSz = (size_t*)ptr - 1;
    extraMemoryAllocated -= *pSz;
    printf("Extra memory deallocated, size: %ld\n", *pSz);
    free((size_t*)ptr - 1);
}

size_t Size(void* ptr)
{
    return ((size_t*)ptr)[-1];
}

// implements heap sort
// extraMemoryAllocated counts bytes of memory allocated
void heapify(int pData[], int n, int i)
{
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < n && pData[left] > pData[largest])
        largest = left;

    if (right < n && pData[right] > pData[largest])
        largest = right;

    if (largest != i)
    {
        int temp = pData[i];
        pData[i] = pData[largest];
        pData[largest] = temp;
        heapify(pData, n, largest);
    }
}

void heapSort(int pData[], int n)
{
    for (int i = (n / 2) - 1; i >= 0; i--)
        heapify(pData, n, i);

    for (int i = n - 1; i >= 0; i--)
    {
        int temp = pData[0];
        pData[0] = pData[i];
        pData[i] = temp;
        heapify(pData, i, 0);
    }
}

// implement merge sort
// extraMemoryAllocated counts bytes of extra memory allocated
void merge(int pData[], int left[], int leftSize, int right[], int rightSize)
{
    int i = 0, j = 0, k = 0;
    while (i < leftSize && j < rightSize)
    {
        if (left[i] < right[j])
            pData[k++] = left[i++];
        else
            pData[k++] = right[j++];
    }

    while (i < leftSize)
        pData[k++] = left[i++];

    while (j < rightSize)
        pData[k++] = right[j++];
}

void mergeSort(int pData[], int l, int r)
{
    if (l < r)
    {
        int m = l + (r - l) / 2;
        int leftSize = m - l + 1;
        int rightSize = r - m;
        int* left = (int*)Alloc(sizeof(int) * leftSize);
        int* right = (int*)Alloc(sizeof(int) * rightSize);

        for (int i = 0; i < leftSize; i++)
            left[i] = pData[l + i];
        for (int j = 0; j < rightSize; j++)
            right[j] = pData[m + 1 + j];

        mergeSort(left, 0, leftSize - 1);
        mergeSort(right, 0, rightSize - 1);
        merge(pData + l, left, leftSize, right, rightSize);
        DeAlloc(left);
        DeAlloc(right);
    }
}

// implement insertion sort
// extraMemoryAllocated counts bytes of memory allocated
void insertionSort(int* pData, int n)
{
    int i, j, temp;
    for (i = 1; i < n; i++)
    {
        temp = pData[i];
        j = i - 1;
        while (j >= 0 && pData[j] > temp)
        {
            pData[j + 1] = pData[j];
            j--;
        }
        pData[j + 1] = temp;
    }
}

// implement bubble sort
// extraMemoryAllocated counts bytes of extra memory allocated
void bubbleSort(int* pData, int n)
{
    int i, j, temp;
    for (i = 0; i < n - 1; i++)
    {
        for (j = 0; j < n - i - 1; j++)
        {
            if (pData[j] > pData[j + 1])
            {
                temp = pData[j];
                pData[j] = pData[j + 1];
                pData[j + 1] = temp;
            }
        }
    }
}

// implement selection sort
// extraMemoryAllocated counts bytes of extra memory allocated
void selectionSort(int* pData, int n)
{
    int i, j, min, temp;
    for (i = 0; i < n - 1; i++)
    {
        min = i;
        for (j = i + 1; j < n; j++)
        {
            if (pData[j] < pData[min])
                min = j;
        }
        temp = pData[i];
        pData[i] = pData[min];
        pData[min] = temp;
    }
}

// parses input file to an integer array
int parseData(char* inputFileName, int** ppData)
{
    FILE* inFile = fopen(inputFileName, "r");
    int dataSz = 0;
    int i, n;
    *ppData = NULL;

    if (inFile)
    {
        fscanf(inFile, "%d\n", &dataSz);
        *ppData = (int*)Alloc(sizeof(int) * dataSz);
        for (i = 0; i < dataSz; i++)
        {
            fscanf(inFile, "%d", &n);
            (*ppData)[i] = n;
        }
        fclose(inFile);
    }

    return dataSz;
}

// prints first and last 100 items in the data array
void printArray(int pData[], int dataSz)
{
    int i, sz = dataSz - 100;
    printf("\tData:\n\t");
    for (i=0;i<100;++i)
    {
        printf("%d ",pData[i]);
    }
    printf("\n\t");

    for (i=sz;i<dataSz;++i)
    {
        printf("%d ",pData[i]);
    }
    printf("\n\n");
}

int main(void)
{
    clock_t start, end;
    int i;
    double cpu_time_used;
    char* fileNames[] = { "input1.txt", "input2.txt", "input3.txt" };

    for (i = 0; i < 3; ++i)
    {
        int* pDataSrc, * pDataCopy;
        int dataSz = parseData(fileNames[i], &pDataSrc);

        if (dataSz <= 0)
            continue;

        pDataCopy = (int*)Alloc(sizeof(int) * dataSz);

        printf("---------------------------\n");
        printf("Dataset Size : %d\n",dataSz);
        printf("---------------------------\n");

        printf("Selection Sort:\n");
        memcpy(pDataCopy, pDataSrc, dataSz * sizeof(int));
        extraMemoryAllocated = 0;
        start = clock();
        selectionSort(pDataCopy, dataSz);
        end = clock();
        cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
        printf("\truntime\t\t\t: %.1lf\n", cpu_time_used);
        printf("\textra memory allocated\t: %d\n", extraMemoryAllocated);
        printArray(pDataCopy, dataSz);

        printf("Insertion Sort:\n");
        memcpy(pDataCopy, pDataSrc, dataSz * sizeof(int));
        extraMemoryAllocated = 0;
        start = clock();
        insertionSort(pDataCopy, dataSz);
        end = clock();
        cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
        printf("\truntime\t\t\t: %.1lf\n", cpu_time_used);
        printf("\textra memory allocated\t: %d\n", extraMemoryAllocated);
        printArray(pDataCopy, dataSz);

        printf("Bubble Sort:\n");
        memcpy(pDataCopy, pDataSrc, dataSz * sizeof(int));
        extraMemoryAllocated = 0;
        start = clock();
        bubbleSort(pDataCopy, dataSz);
        end = clock();
        cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
        printf("\truntime\t\t\t: %.1lf\n", cpu_time_used);
        printf("\textra memory allocated\t: %d\n", extraMemoryAllocated);
        printArray(pDataCopy, dataSz);

        printf("Merge Sort:\n");
        memcpy(pDataCopy, pDataSrc, dataSz * sizeof(int));
        extraMemoryAllocated = 0;
        start = clock();
        mergeSort(pDataCopy, 0, dataSz - 1);
        end = clock();
        cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
        printf("\truntime\t\t\t: %.1lf\n", cpu_time_used);
        printf("\textra memory allocated\t: %d\n", extraMemoryAllocated);
        printArray(pDataCopy, dataSz);

        printf("Heap Sort:\n");
        memcpy(pDataCopy, pDataSrc, dataSz * sizeof(int));
        extraMemoryAllocated = 0;
        start = clock();
        heapSort(pDataCopy, dataSz);
        end = clock();
        cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
        printf("\truntime\t\t\t: %.1lf\n", cpu_time_used);
        printf("\textra memory allocated\t: %d\n", extraMemoryAllocated);
        printArray(pDataCopy, dataSz);

        DeAlloc(pDataCopy);
        DeAlloc(pDataSrc);
    }

}