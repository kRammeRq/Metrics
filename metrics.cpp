#include "metrics.h"
#include <iostream>

namespace Metrics {

    MetricsCollector::MetricsCollector(const std::string& filename)
        : file_(filename, std::ios::app) {
        if (!file_.is_open()) {
            throw std::runtime_error("Failed to open metrics file");
        }
        writer_thread_ = std::thread(&MetricsCollector::writerThread, this);
    }

    MetricsCollector::~MetricsCollector() {
        running_ = false;
        if (writer_thread_.joinable()) {
            writer_thread_.join();
        }
        flush();
    }

    void MetricsCollector::flush() {
        std::lock_guard<std::mutex> lock(mutex_);
        while (!queue_.empty()) {
            const auto& metrics = queue_.front();

            auto now = std::chrono::system_clock::now();
            auto time = std::chrono::system_clock::to_time_t(now);
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                now.time_since_epoch()) % 1000;

            file_ << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
            file_ << '.' << std::setfill('0') << std::setw(3) << ms.count() << " ";

            for (const auto& [name, value] : metrics) {
                file_ << "\"" << name << "\" ";
                std::visit([this](auto&& arg) {
                    file_ << arg << " ";
                    }, value);
            }

            file_ << "\n";
            queue_.pop();
        }
        file_.flush();
    }

    void MetricsCollector::writerThread() {
        while (running_) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            flush();
        }
    }

}