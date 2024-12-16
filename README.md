## ThreadPool
- A threadpool implementation based on Round Robin process scheduling algorithm
- Use features such as `std::mutex`, `std::future`, `std::foward`, `std::atomic` to ensure multi-threaded safety and perfect forwarding
- This threadpool is written based on the C++14 standard. The code refers to the Github threadpool open source project and muduo's threadpool module
## How to use
- When using this, you need to declare `include` at the beginning to include the threadpool library
```cpp
#include"ThreadPool.h"
```
- Create a threadpool in the program and assign the function you want to execute to the threadpool for execution
```cpp
class person{
private:
    double height;
    double weight;
public:
    person(double h_, double w_): height(h_), weight(w_) {}

    double BMI(int index){
        return weight / (height * 2);
    }
};

int main(){
    ThreadPool tp(4);
    std::vector<std::unique_ptr<person>> persons;
    for(int i = 0; i < 20; ++i){
        persons.emplace_back(std::make_unique<person>(1.7 + (i / 100), 70 + i));
    }

    std::vector<std::future<double>> results;
    for(int i = 0; i < 20; ++i){
        results.emplace_back(tp.enqueueTask(
            [person = persons[i].get(), i]() {return person->BMI(i); }
        ));
    }

    for(auto& result : results){
        std::cout << "result is " << result.get() << endl;
    }

    return 0;
}
```
- You can use the `test.cpp` and `test1.cpp` code in the project to test the standard thread pool
## About
- If there are any bugs and any optimization or suggestions, please let me know! XD
- Gmail: msz006incubus@gmail.com