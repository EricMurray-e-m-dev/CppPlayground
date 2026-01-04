#pragma once

#include <cstdint>
#include <memory>
#include <string>

namespace Limiter {

    class RateLimiterImpl;

    // Duration helper for expressing time windows
    struct Duration {
        uint64_t milliseconds;

        static Duration seconds(uint64_t s) { return { s * 1000}; }
        static Duration minutes(uint64_t m) { return { m * 60 * 1000}; }
        static Duration hours(uint64_t h) { return { h * 60 * 60 * 1000}; }
    };

    class RateLimiter {
        public:
            // Constructor/Destructor
            RateLimiter(uint64_t limit, Duration window);
            ~RateLimiter();

            // Move only, disable copy
            RateLimiter(RateLimiter&&) noexcept;
            RateLimiter& operator=(RateLimiter&&) noexcept;
            RateLimiter(const RateLimiter&) = delete;
            RateLimiter& operator=(const RateLimiter&) = delete;

            // Check if req allow, reset key, reset all
            bool allow(const std::string& key);
            void reset(const std::string& key);
            void reset_all();
        
        private:
            std::unique_ptr<RateLimiterImpl> impl_;
    };
}
