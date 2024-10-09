#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <list>  
#include <iostream>
#include <cstring> // For memset

#define MAX 1000  // Adjust this as needed
#define NUM_THREADS 4  // Number of threads (you can adjust this based on your system)

std::list<int> listData;
std::list<int> primeSeeds;
pthread_mutex_t lock;  // Mutex lock for thread synchronization

// Function to compute primes up to sqrt(MAX) sequentially
void SieveOfEratosthenes(int n) {
    bool prime[n + 1];
    memset(prime, true, sizeof(prime));

    for (int p = 2; p * p <= n; p++) {
        if (prime[p]) {
            for (int i = p * p; i <= n; i += p)
                prime[i] = false;
        }
    }

    // Store prime numbers in primeSeeds list
    for (int p = 2; p <= n; p++) {
        if (prime[p]) {
            primeSeeds.push_back(p);
        }
    }
}

// Structure to pass data to each thread
struct ThreadData {
    int start;
    int end;
};

// Function for each thread to execute
void* markPrimes(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    int start = data->start;
    int end = data->end;

    // Create a boolean array to mark numbers as prime or not in this chunk
    bool* primeChunk = new bool[end - start + 1];
    memset(primeChunk, true, sizeof(bool) * (end - start + 1));

    // Use the prime seeds to mark non-prime numbers in this chunk
    for (int prime : primeSeeds) {
        // Find the first multiple of prime in the range [start, end]
        int firstMultiple = std::max(prime * prime, (start + prime - 1) / prime * prime);

        for (int i = firstMultiple; i <= end; i += prime) {
            primeChunk[i - start] = false;
        }
    }

    // Collect primes found by this thread into the global list
    pthread_mutex_lock(&lock); // Acquire the lock to ensure thread-safe access
    for (int i = 0; i <= end - start; i++) {
        if (primeChunk[i]) {
            listData.push_back(i + start);
        }
    }
    pthread_mutex_unlock(&lock); // Release the lock

    delete[] primeChunk;
    pthread_exit(NULL);
}

int main() {
    int sqrtMax = (int)sqrt(MAX);

    // Step 1: Find primes up to sqrt(MAX) sequentially
    SieveOfEratosthenes(sqrtMax);

    pthread_t threads[NUM_THREADS];
    ThreadData threadData[NUM_THREADS];
    int range = (MAX - sqrtMax) / NUM_THREADS;

    // Initialize the mutex
    pthread_mutex_init(&lock, NULL);

    // Step 2: Create threads to process each chunk
    for (int i = 0; i < NUM_THREADS; i++) {
        threadData[i].start = sqrtMax + 1 + i * range;
        threadData[i].end = (i == NUM_THREADS - 1) ? MAX : threadData[i].start + range - 1;
        
        int rc = pthread_create(&threads[i], NULL, markPrimes, (void*)&threadData[i]);
        if (rc) {
            printf("Error: unable to create thread, %d\n", rc);
            exit(-1);
        }
    }

    // Step 3: Wait for all threads to finish
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Clean up mutex
    pthread_mutex_destroy(&lock);

    // Step 4: Print all prime numbers
    listData.sort(); // Sorting the list to ensure primes are in order
    for (int prime : listData) {
        std::cout << prime << " ";
    }
    std::cout << std::endl;

    return 0;
}
