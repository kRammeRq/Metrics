#pragma once

#include <string>
#include <unordered_map>
#include <variant>
#include <chrono>
#include <fstream>
#include <mutex>
#include <thread>
#include <atomic>
#include <queue>
#include <memory>
#include <iomanip>
#include <sstream>

namespace Metrics {

    using Value = std::variant<int, double, std::string>;
    using MetricData = std::unordered_map<std::string, Value>;

    class MetricsCollector {
    public:
        MetricsCollector(const std::string& filename);
        ~MetricsCollector();

        template<typename T>
        void addMetric(const std::string& name, const T& value);

        void flush();

    private:
        void writerThread();

        std::ofstream file_;
        std::mutex mutex_;
        std::queue<MetricData> queue_;
        std::atomic<bool> running_{ true };
        std::thread writer_thread_;
    };

    template<typename T>
    void MetricsCollector::addMetric(const std::string& name, const T& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        MetricData metrics;
        metrics[name] = value;
        queue_.push(metrics);
    }

} 