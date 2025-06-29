#include "logger.hpp"
#include <iostream>

std::thread Logger::s_workerThread;
std::atomic<bool> Logger::s_running{false};
std::queue<std::string> Logger::s_logQueue;
std::mutex Logger::s_mutex;
std::condition_variable Logger::s_cv;
std::vector<std::string> Logger::s_logHistory;

void Logger::init() {
    s_running = true;
    s_workerThread = std::thread(Logger::processLogs);
}

void Logger::shutdown() {
    if (!s_running) return;
    s_running = false;
    s_cv.notify_all();
    if (s_workerThread.joinable()) s_workerThread.join();
}


void Logger::log(const std::string& message) {
    {
        std::lock_guard<std::mutex> lock(s_mutex);
        s_logQueue.push(message);
    }
    s_cv.notify_one();
}

std::vector<std::string> Logger::getBufferedLogs() {
    std::lock_guard<std::mutex> lock(s_mutex);
    return s_logHistory;
}

void Logger::processLogs() {
    try {
        while (s_running) {
            std::unique_lock<std::mutex> lock(s_mutex);
            s_cv.wait(lock, [] { return !s_logQueue.empty() || !s_running; });

            while (!s_logQueue.empty()) {
                std::string msg = s_logQueue.front();
                s_logQueue.pop();
                s_logHistory.push_back(msg);
                // std::cout << msg << std::endl;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "[Logger] Exception in logger thread: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "[Logger] Unknown exception in logger thread" << std::endl;
    }
}

