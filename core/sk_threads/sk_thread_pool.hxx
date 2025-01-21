#pragma once

#include "../sk_common.hxx"

BEGIN_SK_NAMESPACE

using SK_Thread_Pool_Task = std::function<void(std::thread* thread)>;
using SK_Thread_Pool_MainThreadTask = std::function<void()>;
using SK_Thread_Pool_MainThreadRunner = std::function<void(SK_Thread_Pool_MainThreadTask)>;

class SK_Thread_Pool {
public:
    static inline std::thread::id mainThreadId = std::this_thread::get_id();

    static inline bool thisFunctionRunningInMainThread() {
        if (std::this_thread::get_id() == mainThreadId) return true;
        return false;
    };

    // Constructor to initialize the thread pool with a specific size
    explicit SK_Thread_Pool(size_t threadCount = std::thread::hardware_concurrency()) : stop(false) {
        for (size_t i = 0; i < threadCount; ++i) {
            threads.emplace_back([this, i]() {
                std::thread* currentThread;

                try {
                    currentThread = &threads[i]; // Capture pointer to this thread
                }
                catch (int err) {
                    int x = 0;
                };

                while (true) {
                    SK_Thread_Pool_Task task;
                    {
                        std::unique_lock<std::mutex> lock(mtx);
                        cv.wait(lock, [this]() { return stop || !tasks.empty(); });

                        if (stop && tasks.empty())
                            return;

                        // Pick the next available task
                        if (!tasks.empty()) {
                            task = std::move(tasks.front());
                            tasks.erase(tasks.begin()); // Remove the task from the vector
                        }
                    }

                    // Execute the task with the thread pointer
                    if (task) {
                        task(currentThread);
                    }
                }
            });
        }
    }

    // Destructor to stop threads and clean up
    ~SK_Thread_Pool() {
        {
            std::unique_lock<std::mutex> lock(mtx);
            stop = true;
        }
        cv.notify_all(); // Wake up all threads
        for (std::thread& t : threads) {
            if (t.joinable())
                t.join();
        }
    }

    // Function to queue a task
    void queue(SK_Thread_Pool_Task task) {
        {
            std::unique_lock<std::mutex> lock(mtx);
            tasks.push_back(std::move(task)); // Add the task to the vector
        }
        cv.notify_one(); // Notify one thread to handle the task
    }

    void queueOnMainThread(SK_Thread_Pool_MainThreadTask mainThreadTask) {
        std::lock_guard lock(mainThreadMtx);
        mainThreadTasks.push_back(std::move(mainThreadTask));
        mainThreadCv.notify_one();
    }

    // Function to queue an asynchronous task
    void newAsync(std::function<void(SK_Thread_Pool_MainThreadRunner)> callback) {
        queue([this, callback](std::thread* thread) {
            // Provide a way to enqueue tasks on the main thread
            SK_Thread_Pool_MainThreadRunner runOnMainThread = [this](SK_Thread_Pool_MainThreadTask mainThreadTask) {
                queueOnMainThread(mainThreadTask);
            };

            // Run the user-provided callback
            callback(runOnMainThread);
        });
    }

    // Process all main-thread tasks (to be called from the main thread)
    void processMainThreadTasks() {
        std::vector<SK_Thread_Pool_MainThreadTask> localTasks;
        {
            std::lock_guard<std::mutex> lock(mainThreadMtx);
            std::swap(localTasks, mainThreadTasks);
        }

        for (auto& task : localTasks) {
            task();
        }
    }

    // Static method to manage named instances (optional)
    static std::shared_ptr<SK_Thread_Pool> getNamedInstance(const std::string& name, size_t threadCount = std::thread::hardware_concurrency()) {
        static std::mutex instanceMutex;
        static std::unordered_map<std::string, std::shared_ptr<SK_Thread_Pool>> instances;

        std::unique_lock<std::mutex> lock(instanceMutex);
        auto it = instances.find(name);
        if (it == instances.end()) {
            // Create a new instance if it doesn't exist
            auto instance = std::make_shared<SK_Thread_Pool>(threadCount);
            instances[name] = instance;
            return instance;
        }
        return it->second;
    }

private:
    std::vector<std::thread> threads;                  // Vector to hold worker threads
    std::vector<SK_Thread_Pool_Task> tasks;                           // Vector to hold tasks
    std::mutex mtx;                                    // Mutex to protect task list
    std::condition_variable cv;                        // Condition variable for synchronization
    std::atomic<bool> stop;                            // Flag to stop threads

    // Main-thread task management
    std::vector<SK_Thread_Pool_MainThreadTask> mainThreadTasks;       // Vector to hold main-thread tasks
    std::mutex mainThreadMtx;                          // Mutex to protect main-thread task list
    std::condition_variable mainThreadCv;              // Condition variable for main-thread synchronization
};

END_SK_NAMESPACE
