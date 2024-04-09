#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <atomic>

std::mutex mtx;
std::condition_variable cv;
bool ready = false;
std::atomic<int> numThreadsReady = 0;

void worker(int id) {
    std::unique_lock<std::mutex> lck(mtx);
    numThreadsReady++;
    cv.wait(lck, [] { return ready; });

    // Simulate work
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::cout << "Thread " << id << " is working..." << std::endl;
}

int main() {
    const int numThreads = 5;
    std::vector<std::thread> threads;

    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(worker, i);
    }

    // Wait for all threads to be ready before proceeding
    while (numThreadsReady < numThreads)
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Start all threads simultaneously
    {
        std::lock_guard<std::mutex> lck(mtx);
        ready = true;
        cv.notify_all();
    }

    for (auto& t : threads) {
        t.join();
    }

    return 0;
}