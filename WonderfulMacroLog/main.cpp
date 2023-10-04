#include <iostream>
#include <string>
#include <source_location>
#include <format>
#include <cstdint>

#define FOREACH_LOG_LEVEL(f) \
    f(Debug) \
    f(Info) \
    f(Warning) \
    f(Error) \
    f(Fatal)

enum class LogLevel : uint8_t {
#define FUNCTION(name) name,
    FOREACH_LOG_LEVEL(FUNCTION)
#undef FUNCTION
};

std::string to_string(LogLevel level) {
#define FUNCTION(name) case LogLevel::name: return #name;
    switch (level) {
        FOREACH_LOG_LEVEL(FUNCTION)
    }
    return "Unknown";
#undef FUNCTION
}

static LogLevel minLevel = LogLevel::Info;

template<typename T>
class WithSourceLocation {
public:
    template<class U>
    requires std::constructible_from<T, U>
    consteval WithSourceLocation(U &&inner, std::source_location loc = std::source_location::current())
            : inner(std::forward<U>(inner)), loc(loc) {};

    constexpr T const &format() const { return inner; }

    constexpr std::source_location const &location() const { return loc; }

private:
    T inner;
    std::source_location loc;
};

template<typename... Args>
void genericLog(LogLevel lev, WithSourceLocation<std::format_string<Args...>> fmt, Args &&... args) {
    if (lev < minLevel) {
        return;
    }
    auto const &loc = fmt.location();
    std::cout << loc.file_name() << ":" << loc.line() << " [" << to_string(lev) << "] "
              << std::vformat(fmt.format().get(), std::make_format_args(args...)) << std::endl;
}

#define FUNCTION(name) \
    template<typename... Args> \
    void log##name(WithSourceLocation<std::format_string<Args...>> fmt, Args &&... args) { \
            return genericLog(LogLevel::name, std::move(fmt), std::forward<Args>(args)...);\
            }

FOREACH_LOG_LEVEL(FUNCTION)

#undef FUNCTION

int main() {
//    int x = std::cin.get();
    logDebug("Hello, world!, {}, {}", 1, 2);
    logInfo("Hello, world!, {}, {}", 3, 4);
    logError("Hello, world!, {}, {}", 5, 6);
    return 0;
}
