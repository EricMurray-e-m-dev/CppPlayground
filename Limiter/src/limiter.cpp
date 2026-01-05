#include "limiter/limiter.hpp"
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

namespace Limiter {

    // Per-key bucket
    struct Bucket {
        double tokens;      // Available tokens
        std::chrono::steady_clock::time_point last_refill;
    };

    class RateLimiterImpl {
        public:
            RateLimiterImpl(uint64_t limit, Duration window)
                : limit_(static_cast<double>(limit))
                , refill_rate_(static_cast<double>(limit) / (window.milliseconds / 1000.0))
            {}

            bool allow(const std::string& key) {
                std::lock_guard<std::mutex> lock(mutex_);

                auto now = std::chrono::steady_clock::now();
                auto& bucket = buckets_[key];

                // First req for key init bucket
                if (bucket.tokens == 0.0 && bucket.last_refill.time_since_epoch().count() == 0) {
                    bucket.tokens = limit_;
                    bucket.last_refill = now;
                }

                // Refill tokens based on time
                refill(bucket, now);

                // Check if req allowed
                if (bucket.tokens >= 1.0) {
                    bucket.tokens -= 1.0;
                    return true;
                }
                return false;
            }

            void reset(const std::string& key) {
                std::lock_guard<std::mutex> lock(mutex_);
                buckets_.erase(key);
            }

            void reset_all() {
                std::lock_guard<std::mutex> lock(mutex_);
                buckets_.clear();
            }

        private:
            void refill(Bucket& bucket, std::chrono::steady_clock::time_point now) {
                // Calc time since last refill
                auto elapsed = std::chrono::duration<double>(now - bucket.last_refill).count();

                // Add tokens based off refill rate
                double tokens_to_add = elapsed * refill_rate_;
                bucket.tokens = std::min(bucket.tokens + tokens_to_add, limit_);
                bucket.last_refill = now;
            }

            double limit_;
            double refill_rate_;
            std::unordered_map<std::string, Bucket> buckets_;
            std::mutex mutex_;
    };

    RateLimiter::RateLimiter(uint64_t limit, Duration window)
        : impl_(std::make_unique<RateLimiterImpl>(limit, window))
    {}

    RateLimiter::~RateLimiter() = default;

    RateLimiter::RateLimiter(RateLimiter&&) noexcept = default;
    RateLimiter& RateLimiter::operator=(RateLimiter&&) noexcept = default;

    bool RateLimiter::allow(const std::string& key) {
        return impl_->allow(key);
    }

    void RateLimiter::reset(const std::string& key) {
        impl_->reset(key);
    }

    void RateLimiter::reset_all() {
        impl_->reset_all();
    }
}