#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <vector>

class Semaphore {
private:
    std::mutex mtx;
    std::condition_variable cv;
    int count;

public:
    Semaphore(int count_ = 0) : count(count_) {}
    
    void notify() {
        std::unique_lock<std::mutex> lock(mtx);
        ++count;
        cv.notify_one();
    }

    void wait() {
        std::unique_lock<std::mutex> lock(mtx);
        while(count == 0) {
            cv.wait(lock);
        }
        --count;
    }
};

std::queue<int> messageQueue;
std::mutex queueMutex;
Semaphore itemsAvailable(0);

void producer(int id) {
    for (int i = 0; i < 10; ++i) {
        std::unique_lock<std::mutex> lock(queueMutex);
        std::cout << "Producer " << id << " produced message " << i << std::endl;
        messageQueue.push(i);
        lock.unlock();
        
        itemsAvailable.notify();
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void consumer(int id) {
    while (true) {
        itemsAvailable.wait();
        
        std::unique_lock<std::mutex> lock(queueMutex);
        if (!messageQueue.empty()) {
            int message = messageQueue.front();
            messageQueue.pop();
            std::cout << "Consumer " << id << " consumed message " << message << std::endl;
        }
        lock.unlock();
    }
}

int main() {
    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;

    // 创建两个生产者线程
    for (int i = 0; i < 2; ++i) {
        producers.push_back(std::thread(producer, i));
    }

    // 创建四个消费者线程
    for (int i = 0; i < 4; ++i) {
        consumers.push_back(std::thread(consumer, i));
    }

    // 等待所有生产者线程完成
    for (auto& t : producers) {
        t.join();
    }

    // 在这个示例中，消费者线程是永久运行的，所以我们不会等待它们完成。
    // 在实际应用中，您可能需要一种机制来优雅地关闭消费者线程。

    return 0;
}
