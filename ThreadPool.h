#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>
#include <future>

class ThreadPool{
public:
    ThreadPool(size_t numThreads);
    ~ThreadPool();

    template<class T, class... Args>
    auto enqueueTask(T&& func, Args&&... args) -> std::future<decltype(func(args...))>;

private:
    void worker(size_t index);

    size_t numThreads;
    std::vector<std::thread> threads;
    std::vector<std::queue<std::function<void()>>> threadQueues;

    std::mutex thread_mutex;
    std::condition_variable cv;
    std::atomic<size_t> nextThreadIndex;
    std::atomic<bool> stop;
};

ThreadPool::ThreadPool(size_t numThreads): numThreads(numThreads), 
nextThreadIndex(0), stop(false), threadQueues(numThreads){
    for(size_t i = 0; i < numThreads; ++i){
        threads.emplace_back(&ThreadPool::worker, this, i);
    }
}

ThreadPool::~ThreadPool(){
    stop.store(true);
    cv.notify_all();

    for(auto& thread : threads){
        if(thread.joinable()){
            thread.join();
        }
    }
}

template <class T, class... Args>
auto ThreadPool::enqueueTask(T&& func, Args&&... args) -> std::future<decltype(func(args...))> {
    using returnType = decltype(func(args...));

    auto task = std::make_shared<std::packaged_task<returnType()>>(
        std::bind(std::forward<T>(func), std::forward<Args>(args)...)
    );

    std::future<returnType> result = task->get_future();

    {
        std::lock_guard<std::mutex> lock(thread_mutex);
        if(stop.load()){
            throw std::runtime_error("Task push fails, Threadpool is already stop");
        }
        size_t index = nextThreadIndex.fetch_add(1) % numThreads;
        threadQueues[index].push([task]() {(*task)(); });
    }

    cv.notify_all();
    return result;
}

void ThreadPool::worker(size_t index){
    while(true){
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(thread_mutex);
            cv.wait(lock, [this, index] {
                return stop.load() || !threadQueues[index].empty();
            });

            if(stop.load() && threadQueues[index].empty()) return;
            task = std::move(threadQueues[index].front());
            threadQueues[index].pop();
        }
        task();
    }
}