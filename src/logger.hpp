#pragma once
#include <string>
#include <thread>
#include <atomic>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <vector>

class Logger {
public:
    static void init();
    static void shutdown();
    static void log(const std::string& message);
    static std::vector<std::string> getBufferedLogs();

private:
    static void processLogs();

    static std::thread s_workerThread;
    static std::atomic<bool> s_running;
    static std::queue<std::string> s_logQueue;
    static std::mutex s_mutex;
    static std::condition_variable s_cv;
    static std::vector<std::string> s_logHistory;
};
