#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <queue>

class Client;

class Strategy {
public:
    Strategy(Client* client);

    virtual void onMessage(const std::string& message) = 0;

    std::atomic<bool> status{false};

protected:
    Client* client;
};

class Client {
public:
    void subscribe(Strategy* strategy) {
        strategies.push_back(strategy);
    }

    void receiveMessage(const std::string& message) {
        // 找到未在回调中的策略，通知它们
        for (size_t i = 0; i < strategies.size(); ++i) {
            if (!strategies[i]->status) {
                strategies[i]->status = true; // 设置标志为“正在回调中”
                std::thread([this, i, message]() {
                    strategies[i]->onMessage(message);
                    strategies[i]->status = false; // 设置标志为“未在回调中”
                }).detach(); // 使用detach来分离线程，避免std::async的问题
            }
        }
    }

private:
    std::vector<Strategy*> strategies;
    std::mutex mutex;

};

Strategy::Strategy(Client* client) : client(client) {
    client->subscribe(this);
}

class Strategy1 : public Strategy {
public:
    Strategy1(Client* client) : Strategy(client) {}

    void onMessage(const std::string& message) override {
        // 模拟非阻塞回调
        std::cout << "Strategy 1 received message: " << message << std::endl;
        std::this_thread::sleep_for(std::chrono::microseconds(200));
    }
};

class Strategy2 : public Strategy {
public:
    Strategy2(Client* client) : Strategy(client) {}

    void onMessage(const std::string& message) override {
        // 模拟非阻塞回调
        std::cout << "Strategy 2 received message: " << message << std::endl;
    }
};

int main() {
    Client client;
    Strategy1 strategy1(&client);
    Strategy2 strategy2(&client);

    // 模拟收到新消息
    for (int i = 1; i <= 50; ++i) {
        std::string message = "New market data " + std::to_string(i);
        client.receiveMessage(message);
        std::this_thread::sleep_for(std::chrono::microseconds(10));

    }

    // 等待消息处理完毕
    std::this_thread::sleep_for(std::chrono::seconds(3));

    return 0;
}
