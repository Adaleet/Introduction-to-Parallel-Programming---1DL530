#include <atomic>
#include <vector>
#include <thread>
#include <iostream>

std::vector<int> data(10);
std::vector<std::atomic<bool>> fine_grained_locks(10); // An atomic flag for each element

void tatas_lock(int index) {
    while (true) {
        while (fine_grained_locks[index].load(std::memory_order_relaxed));
        if (!fine_grained_locks[index].exchange(true, std::memory_order_acquire)) {
            break; // Successfully acquired the lock
        }
    }
}

void tatas_unlock(int index) {
    fine_grained_locks[index].store(false, std::memory_order_release);
}

void increment_element(int index) {
    tatas_lock(index); // Acquire lock for specific element
    ++data[index]; // Critical section
    tatas_unlock(index); // Release lock
}

int main() {
    std::thread t1(increment_element, 1);
    std::thread t2(increment_element, 1);

    t1.join();
    t2.join();

    std::cout << "Data at index 1: " << data[1] << std::endl;
}
