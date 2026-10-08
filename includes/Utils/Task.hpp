#include <vector>
#include <functional>
#include <mutex>
#include <queue>
#include <future>
#include "raylib.h"

struct ScheduledTask {
    double timeToExecute;
    std::function<void()> action;
};


class Task {
private:
    inline static std::vector<ScheduledTask> m_queue = {};

    inline static std::queue<std::function<void()>> m_mainThreadRequests = {};
    inline static std::mutex m_mutex;
public:
    static void delay(float seconds, std::function<void()> action) {
        m_queue.push_back({ GetTime() + (double)seconds, action });
    }

    static void spawn(std::function<void()> action) {
        std::thread t([action]() {
            action();
        });
        t.detach();
    }

    static void sync(std::function<void()> action) {
        // Create a promise/future pair
        auto promise = std::make_shared<std::promise<void>>();
        std::future<void> future = promise->get_future();

        // Wrap the action so it signals the promise when finished
        runOnMainThread([action, promise]() {
            action();
            promise->set_value(); // Signal the background thread to continue
        });

        // The background thread stops here and waits!
        future.wait();
    }

    static void runOnMainThread(std::function<void()> action) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_mainThreadRequests.push(action);
    }

    static void update() {
        std::lock_guard<std::mutex> lock(m_mutex);
        while (!m_mainThreadRequests.empty()) {
            m_mainThreadRequests.front()();
            m_mainThreadRequests.pop();
        }

        double now = GetTime();
        for (auto it = m_queue.begin(); it != m_queue.end(); )
        {
            if (now >= it->timeToExecute) {
                it->action();
                it = m_queue.erase(it);
            } else {
                ++it;
            }
        }
    }
};