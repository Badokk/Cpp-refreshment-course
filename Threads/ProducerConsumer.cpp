#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>

namespace threads {
namespace producerconsumer {

using namespace std;

void oneProducerOneConsumer() {
    queue<int> productionQueue;
    const int maxSize = 10;
    condition_variable cvConsumption;
    condition_variable cvProduction;
    mutex m;

    auto consumer = [&productionQueue, &cvConsumption, &cvProduction, &m](){
    cout << "<producer consumer>" << endl;
        int eaten = 0;
        cout << "Consumer start " << endl;

        while (eaten < 200) {
            std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 500 + 2));
            unique_lock<mutex> consumptionLock(m);
    
            if (productionQueue.size() > 0)
            {
                eaten++;
                cout << "Consuming " << productionQueue.front() << endl;
                productionQueue.pop();
            }
            else;
                cvConsumption.wait(consumptionLock);
            if (productionQueue.size() == maxSize - 1)
                cvProduction.notify_one();
        }
    };

    auto producer = [&productionQueue, &cvConsumption, &cvProduction, &m](){
        int made = 0;
        cout << "Producer start " << endl;

        while (made < 200) {
            std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 5 + 2));
            unique_lock<mutex> productionLock(m);

            if (productionQueue.size() < maxSize) {
                productionQueue.push(made++);
                cout << "Producing " << made << endl;
            }
            else {
                cout << "Cant produce " << made << endl;
                cvProduction.wait(productionLock);
                cout << "Wait done " << endl;
            }
            if (productionQueue.size() == 1);
                cvConsumption.notify_one();
        }
    };

    
    thread c(consumer);
    thread p(producer);
    c.join();
    p.join();
    cout << "</producer consumer>" << endl;
}

void multiProCo(int numOfPcPairs) {
    cout << "</producers consumers>" << endl;
    queue<int> productionQueue;
    const int maxSize = 10;
    condition_variable cvConsumption;
    condition_variable cvProduction;
    vector<thread> consumers;
    vector<thread> producers;
    mutex m;

    auto consumer = [&productionQueue, &cvConsumption, &cvProduction, &m](){
        int eaten = 0;  
        cout << "Consumer start " << endl;

        while (eaten < 100) {
            std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 200 + 2));
            unique_lock<mutex> consumptionLock(m);
    
            if (productionQueue.size() > 0)
            {
                eaten++;
                cout << "Consuming " << productionQueue.front() << endl;
                productionQueue.pop();
            }
            else
                cvConsumption.wait(consumptionLock);
            if (productionQueue.size() == maxSize - 1)
                cvProduction.notify_all();
        }
    };

    auto producer = [&productionQueue, &cvConsumption, &cvProduction, &m](int x){
        int made = x;
        cout << "Producer start " << endl;

        while (made < 100 + x) {
            std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 5 + 2));
            unique_lock<mutex> productionLock(m);

            if (productionQueue.size() < maxSize) {
                productionQueue.push(made++);
                cout << "Producing " << made << endl;
            }
            else {
                cout << "Cant produce " << made << endl;
                cvProduction.wait(productionLock);
                cout << "Wait done " << endl;
            }
            if (productionQueue.size() == 1);
                cvConsumption.notify_all();
        }
    };

    for (int i = 0; i < numOfPcPairs; i++) consumers.emplace_back(consumer);
    for (int i = 0; i < numOfPcPairs; i++) producers.emplace_back(producer, i*100);
    for (int i = 0; i < numOfPcPairs; i++) consumers[i].join();
    for (int i = 0; i < numOfPcPairs; i++) producers[i].join();
    cout << "</producers consumers>" << endl;
}

}
}
