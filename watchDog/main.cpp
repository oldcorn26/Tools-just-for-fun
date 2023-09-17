#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <vector>
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include "nlohmann/json.hpp"

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
namespace ssl = boost::asio::ssl;
using tcp = boost::asio::ip::tcp;

class WebSocketClient {
public:
    WebSocketClient(const std::string &host, const std::string &port, const std::string &listenKey);

    ~WebSocketClient();

    void connect();

    void send(const std::string &message);

    nlohmann::json read();

    void close();

private:
    void load_root_certificates(ssl::context &ctx);

    std::string host_;
    std::string port_;
    std::string listenKey_;
    net::io_context ioc_;
    ssl::context ctx_;
    websocket::stream <beast::ssl_stream<tcp::socket>> ws_;
};

WebSocketClient::WebSocketClient(const std::string &host, const std::string &port, const std::string &listenKey)
        : host_(host), port_(port), listenKey_(listenKey), ioc_(), ctx_(ssl::context::tlsv12_client), ws_(ioc_, ctx_) {
    load_root_certificates(ctx_);
    connect();
}

WebSocketClient::~WebSocketClient() {
    close();
}

void WebSocketClient::connect() {
    tcp::resolver resolver{ioc_};
    auto const results = resolver.resolve(host_, port_);
    auto ep = net::connect(get_lowest_layer(ws_), results);

    if (!SSL_set_tlsext_host_name(ws_.next_layer().native_handle(), host_.c_str())) {
        beast::error_code ec{static_cast<int>(::ERR_get_error()), net::error::get_ssl_category()};
        throw beast::system_error{ec};
    }

    std::string handshake_host = host_ + ':' + std::to_string(ep.port());
    std::string handshake_endpoint = "/ws";
    if (!listenKey_.empty()) {
        handshake_endpoint += "/" + listenKey_;
    }

    ws_.next_layer().handshake(ssl::stream_base::client);
    ws_.handshake(handshake_host, handshake_endpoint);
}

void WebSocketClient::send(const std::string &message) {
    ws_.write(net::buffer(message));
}

nlohmann::json WebSocketClient::read() {
    beast::flat_buffer buffer;
    ws_.read(buffer);

    auto data = buffer.data();
    auto pointer = static_cast<const char *>(data.data());
    auto len = data.size();
    auto ret = nlohmann::json::parse(pointer, pointer + len);

    buffer.consume(buffer.size());
    return ret;
}

// void WebSocketClient::read() {
//     beast::flat_buffer buffer;
//     ws_.read(buffer);
//     std::cout << beast::make_printable(buffer.data()) << std::endl;
//     buffer.consume(buffer.size());
// }

void WebSocketClient::close() {
    ws_.close(websocket::close_code::normal);
}

void WebSocketClient::load_root_certificates(ssl::context &ctx) {
    std::string const cert = "-----BEGIN CERTIFICATE-----\n"
                             "MIIH0zCCBbugAwIBAgIIXsO3pkN/pOAwDQYJKoZIhvcNAQEFBQAwQjESMBAGA1UE\n"
                             "AwwJQUNDVlJBSVoxMRAwDgYDVQQLDAdQS0lBQ0NWMQ0wCwYDVQQKDARBQ0NWMQsw\n"
                             "CQYDVQQGEwJFUzAeFw0xMTA1MDUwOTM3MzdaFw0zMDEyMzEwOTM3MzdaMEIxEjAQ\n"
                             "BgNVBAMMCUFDQ1ZSQUlaMTEQMA4GA1UECwwHUEtJQUNDVjENMAsGA1UECgwEQUND\n"
                             "VjELMAkGA1UEBhMCRVMwggIiMA0GCSqGSIb3DQEBAQUAA4ICDwAwggIKAoICAQCb\n"
                             "qau/YUqXry+XZpp0X9DZlv3P4uRm7x8fRzPCRKPfmt4ftVTdFXxpNRFvu8gMjmoY\n"
                             "HtiP2Ra8EEg2XPBjs5BaXCQ316PWywlxufEBcoSwfdtNgM3802/J+Nq2DoLSRYWo\n"
                             "G2ioPej0RGy9ocLLA76MPhMAhN9KSMDjIgro6TenGEyxCQ0jVn8ETdkXhBilyNpA\n"
                             "lHPrzg5XPAOBOp0KoVdDaaxXbXmQeOW1tDvYvEyNKKGno6e6Ak4l0Squ7a4DIrhr\n"
                             "IA8wKFSVf+DuzgpmndFALW4ir50awQUZ0m/A8p/4e7MCQvtQqR0tkw8jq8bBD5L/\n"
                             "0KIV9VMJcRz/RROE5iZe+OCIHAr8Fraocwa48GOEAqDGWuzndN9wrqODJerWx5eH\n"
                             "k6fGioozl2A3ED6XPm4pFdahD9GILBKfb6qkxkLrQaLjlUPTAYVtjrs78yM2x/47\n"
                             "4KElB0iryYl0/wiPgL/AlmXz7uxLaL2diMMxs0Dx6M/2OLuc5NF/1OVYm3z61PMO\n"
                             "m3WR5LpSLhl+0fXNWhn8ugb2+1KoS5kE3fj5tItQo05iifCHJPqDQsGH+tUtKSpa\n"
                             "cXpkatcnYGMN285J9Y0fkIkyF/hzQ7jSWpOGYdbhdQrqeWZ2iE9x6wQl1gpaepPl\n"
                             "uUsXQA+xtrn13k/c4LOsOxFwYIRKQ26ZIMApcQrAZQIDAQABo4ICyzCCAscwfQYI\n"
                             "KwYBBQUHAQEEcTBvMEwGCCsGAQUFBzAChkBodHRwOi8vd3d3LmFjY3YuZXMvZmls\n"
                             "ZWFkbWluL0FyY2hpdm9zL2NlcnRpZmljYWRvcy9yYWl6YWNjdjEuY3J0MB8GCCsG\n"
                             "AQUFBzABhhNodHRwOi8vb2NzcC5hY2N2LmVzMB0GA1UdDgQWBBTSh7Tj3zcnk1X2\n"
                             "VuqB5TbMjB4/vTAPBgNVHRMBAf8EBTADAQH/MB8GA1UdIwQYMBaAFNKHtOPfNyeT\n"
                             "VfZW6oHlNsyMHj+9MIIBcwYDVR0gBIIBajCCAWYwggFiBgRVHSAAMIIBWDCCASIG\n"
                             "CCsGAQUFBwICMIIBFB6CARAAQQB1AHQAbwByAGkAZABhAGQAIABkAGUAIABDAGUA\n"
                             "cgB0AGkAZgBpAGMAYQBjAGkA8wBuACAAUgBhAO0AegAgAGQAZQAgAGwAYQAgAEEA\n"
                             "QwBDAFYAIAAoAEEAZwBlAG4AYwBpAGEAIABkAGUAIABUAGUAYwBuAG8AbABvAGcA\n"
                             "7QBhACAAeQAgAEMAZQByAHQAaQBmAGkAYwBhAGMAaQDzAG4AIABFAGwAZQBjAHQA\n"
                             "cgDzAG4AaQBjAGEALAAgAEMASQBGACAAUQA0ADYAMAAxADEANQA2AEUAKQAuACAA\n"
                             "QwBQAFMAIABlAG4AIABoAHQAdABwADoALwAvAHcAdwB3AC4AYQBjAGMAdgAuAGUA\n"
                             "czAwBggrBgEFBQcCARYkaHR0cDovL3d3dy5hY2N2LmVzL2xlZ2lzbGFjaW9uX2Mu\n"
                             "aHRtMFUGA1UdHwROMEwwSqBIoEaGRGh0dHA6Ly93d3cuYWNjdi5lcy9maWxlYWRt\n"
                             "aW4vQXJjaGl2b3MvY2VydGlmaWNhZG9zL3JhaXphY2N2MV9kZXIuY3JsMA4GA1Ud\n"
                             "DwEB/wQEAwIBBjAXBgNVHREEEDAOgQxhY2N2QGFjY3YuZXMwDQYJKoZIhvcNAQEF\n"
                             "BQADggIBAJcxAp/n/UNnSEQU5CmH7UwoZtCPNdpNYbdKl02125DgBS4OxnnQ8pdp\n"
                             "D70ER9m+27Up2pvZrqmZ1dM8MJP1jaGo/AaNRPTKFpV8M9xii6g3+CfYCS0b78gU\n"
                             "JyCpZET/LtZ1qmxNYEAZSUNUY9rizLpm5U9EelvZaoErQNV/+QEnWCzI7UiRfD+m\n"
                             "AM/EKXMRNt6GGT6d7hmKG9Ww7Y49nCrADdg9ZuM8Db3VlFzi4qc1GwQA9j9ajepD\n"
                             "vV+JHanBsMyZ4k0ACtrJJ1vnE5Bc5PUzolVt3OAJTS+xJlsndQAJxGJ3KQhfnlms\n"
                             "tn6tn1QwIgPBHnFk/vk4CpYY3QIUrCPLBhwepH2NDd4nQeit2hW3sCPdK6jT2iWH\n"
                             "7ehVRE2I9DZ+hJp4rPcOVkkO1jMl1oRQQmwgEh0q1b688nCBpHBgvgW1m54ERL5h\n"
                             "I6zppSSMEYCUWqKiuUnSwdzRp+0xESyeGabu4VXhwOrPDYTkF7eifKXeVSUG7szA\n"
                             "h1xA2syVP1XgNce4hL60Xc16gwFy7ofmXx2utYXGJt/mwZrpHgJHnyqobalbz+xF\n"
                             "d3+YJ5oyXSrjhO7FmGYvliAd3djDJ9ew+f7Zfc3Qn48LFFhRny+Lwzgt3uiP1o2H\n"
                             "pPVWQxaZLPSkVrQ0uGE3ycJYgBugl6H8WY3pEfbRD0tVNEYqi4Y7\n"
                             "-----END CERTIFICATE-----\n";

    ctx.add_certificate_authority(boost::asio::buffer(cert.data(), cert.size()));
}

class WatchDog {
public:
    WatchDog(boost::asio::io_context &ioc, std::function<void()> callback);

    void Enable(int timeout);

    void Disable();

    void Reset();
private:
    int timeout_;
    boost::asio::deadline_timer timer_;
    std::function<void()> callback_;
};

WatchDog::WatchDog(boost::asio::io_context &ioc, std::function<void()> callback)
        : timer_(ioc), callback_(callback), timeout_(0) {}

void WatchDog::Enable(int timeout) {
    timeout_ = timeout;
    timer_.cancel();
    timer_.expires_from_now(boost::posix_time::seconds(timeout_));
    timer_.async_wait([this](const boost::system::error_code& ec) {
        if (ec == boost::asio::error::operation_aborted) {
            return;
        }
        callback_();
        this->Reset();
    });
}

void WatchDog::Disable() {
    timer_.cancel();
}

void WatchDog::Reset() {
    Enable(timeout_);
}

void timeoutCallback1() {
    std::cout << "1 Timer has expired!" << std::endl;
}

void timeoutCallback2() {
    std::cout << "2 Timer has expired!" << std::endl;
}

int main() {
    boost::asio::io_context ioc;
    WatchDog dog1(ioc, timeoutCallback1);
    WatchDog dog2(ioc, timeoutCallback2);
    dog1.Enable(1);
    dog2.Enable(2);
    std::thread io_thread([&ioc](){ ioc.run(); });

    WebSocketClient client("fstream.binance.com", "443", "");
    client.send(R"({"method": "SUBSCRIBE","params": ["btcusdt@bookTicker"],"id": 1})");
    while (true) {
        auto response = client.read();
        std::cout << response << std::endl;
    }

    io_thread.join();

    return 0;
}
