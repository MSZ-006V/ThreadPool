#include<iostream>
#include<string>
#include"ThreadPool.h"

using namespace std;

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