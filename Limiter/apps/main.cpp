#include <chrono>
#include <iostream>
#include <limiter/limiter.hpp>
#include <thread>


int main() {
    // 5 reqs per second
    Limiter::RateLimiter limiter(5, Limiter::Duration::seconds(1));

    std::cout << "Testing rate limiter: 5 req/sec\n\n";

    for (int i = 0; i <= 10; ++i) {
        bool allowed = limiter.allow("user_123");

        std::cout << "Request " << i << ": "
                  << (allowed ? "ALLOWED" : "BLOCKED") << "\n";
    }

    std::cout << "\nWaiting 1 second...\n\n";
    std::this_thread::sleep_for(std::chrono::seconds(1));

    for (int i = 11; i <= 15; ++i) {
        bool allowed = limiter.allow("user_123");

        std::cout << "Request " << i << ": "
                  << (allowed ? "ALLOWED" : "BLOCKED") << "\n";
    }

    return 0;
}
