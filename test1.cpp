#include<iostream>
#include<string>
#include"ThreadPool.h"

using namespace std;

int main(){
    ThreadPool tp(4);
    auto func = [](int a, int b, int index){
        int c = a + b;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        // std::cout << "Task " << index << " executed by thread: " 
        // << std::this_thread::get_id() << ", result is " << a + b << std::endl;
        return c;
    };

    std::vector<std::future<int>> results;

    for(int i = 0; i < 5; ++i){
        results.emplace_back(tp.enqueueTask(func, i + 1, i + 2, i));
    }

    for(auto& result : results){
        std::cout << "result is " << result.get() << endl;
    }

    return 0;
}