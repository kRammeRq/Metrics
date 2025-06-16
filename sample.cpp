#include "metrics.h"
#include <thread>
#include <random>

using namespace Metrics;

void simulateCPU(MetricsCollector& collector) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 4.0); 

    while (true) {
        double cpu_load = dis(gen);
        collector.addMetric("CPU", cpu_load);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void simulateHTTP(MetricsCollector& collector) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 100);

    while (true) {
        int requests = dis(gen);
        collector.addMetric("HTTP requests RPS", requests);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main() {
    try {
        MetricsCollector collector("metrics.log");

        std::thread cpu_thread(simulateCPU, std::ref(collector));
        std::thread http_thread(simulateHTTP, std::ref(collector));

        // Дополнительная метрика
        int counter = 0;
        while (true) {
            collector.addMetric("Custom counter", counter++);
            collector.addMetric("Status", "Running");
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }

        cpu_thread.join();
        http_thread.join();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}