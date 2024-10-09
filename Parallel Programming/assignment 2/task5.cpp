#include <atomic>
#include <thread>
#include <iostream>

struct MCSNode {
    std::atomic<MCSNode*> next{nullptr};
    std::atomic<bool> locked{false};
};

std::atomic<MCSNode*> tail{nullptr};

void mcs_lock(MCSNode* node) {
    MCSNode* old_tail = tail.exchange(node);
    if (old_tail != nullptr) {
        node->locked.store(true);
        old_tail->next.store(node);
        while (node->locked.load());
    }
}

void mcs_unlock(MCSNode* node) {
    MCSNode* next = node->next.load();
    if (next == nullptr) {
        if (tail.compare_exchange_strong(node, nullptr)) {
            return;
        }
        while ((next = node->next.load()) == nullptr);
    }
    next->locked.store(false);
}

int shared_data = 0;

void increment_data() {
    MCSNode node;
    mcs_lock(&node);
    ++shared_data; // Critical section
    mcs_unlock(&node);
}

int main() {
    std::thread t1(increment_data);
    std::thread t2(increment_data);

    t1.join();
    t2.join();

    std::cout << "Shared Data: " << shared_data << std::endl;
}


