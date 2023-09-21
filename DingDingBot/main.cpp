#include <unordered_map>
#include <future>
#include "rest_client.h"

class MessageBot {
public:
    MessageBot(const std::string &h, const std::string &p, const std::string &token)
            : host(h), port(p), client(h, p), accessToken(token) {
        body = "/robot/send?access_token=" + accessToken;
    }

    MessageBot(const MessageBot&) = delete;
    
    MessageBot& operator=(const MessageBot&) = delete;

    void sendMessage(const std::string &title, const std::string &text);

private:
    std::string host;
    std::string port;
    std::string accessToken;
    std::string body;
    Client client;
    std::unordered_map<std::string, std::chrono::system_clock::time_point> titleLastSent;
    std::unordered_map<std::string, int> suppressedMessages;

    bool shouldSuppress(const std::string &title);
};

bool MessageBot::shouldSuppress(const std::string &title) {
    const auto now = std::chrono::system_clock::now();
    static const auto hour = std::chrono::minutes(60);

    if (titleLastSent.find(title) != titleLastSent.end() &&
        (now - titleLastSent[title] < hour)) {
        ++suppressedMessages[title];
        return true;
    }

    titleLastSent[title] = now;
    return false;
}

void MessageBot::sendMessage(const std::string &title, const std::string &text) {
    if (shouldSuppress(title)) {
        return;
    }

    http::request <http::string_body> req(http::verb::post, body, 11);
    req.set(http::field::host, host);
    req.set(http::field::content_type, "application/json");
    req.set(http::field::accept_encoding, "gzip, deflate");

    nlohmann::json msg = {
            {"msgtype", "text"},
            {"text",    {
                                {"content",
                                        title + ": " + text + ", 抑制" + std::to_string(suppressedMessages[title]) + "条"}
                        }}
    };
    suppressedMessages[title] = 0;

    req.body() = msg.dump();
    req.prepare_payload();

    auto future = std::async(std::launch::async, [this, &req] {
        client.performRequestSync(req);
    });
}
