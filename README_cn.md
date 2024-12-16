## ThreadPool
- 一个基于Round Robin的线程池实现
- 使用`std::mutex`，`std::future`，`std::foward`，`std::atomic`等特性保证多线程安全和完美转发
- 该内存池基于C++14标准编写，代码参考Github线程池开源项目与muduo的线程池模块
## 使用用例
- 在使用时，在开头声明，包含该线程池库
```cpp
#include"ThreadPool.h"
```
- 在程序中创建线程池，并将想要执行的函数分配给线程池进行执行
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
- 可以使用项目中的`test.cpp`和`test1.cpp`代码测试标准线程池
## 关于代码
- 如果有任何bug和任何优化方案，修改建议，请告诉我！
- Gmail: msz006incubus@gmail.com
- QQ: 49353914
