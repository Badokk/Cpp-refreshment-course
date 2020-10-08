

#include <chrono>
#include <condition_variable>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>


using namespace std;

void naiveCounter();
void guardedCounter();

namespace philosophers {
void naiveSolution();
}
namespace producerconsumer {
void onePrOneCo();
void multiProCo();
}
namespace sort {
void mergeSort(vector<int>& toSort);
}

int x = 0;

int main() {
    cout << "Hello" << endl;

    // naiveCounter();
    // x = 0;
    // guardedCounter();
    // philosophers::naiveSolution();
    // producerconsumer::onePrOneCo();
    // producerconsumer::multiProCo();

    vector<int> toSort{41,13,27,6,18,40,4,21};
    for (auto x : toSort) 
        cout << x << " ";
    cout << endl;

    sort::mergeSort(toSort);

    for (auto x : toSort) 
        cout << x << " ";
    cout << endl;
}

void naiveCounter() {
    cout << "<naive counter>" << endl;
    auto counter = [](){
        cout << "Thread " << this_thread::get_id() << " started" << endl;
        for (int i = 0; i < 2500; i++) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            auto y = x;
            x = y + 1;
        }
        cout << "Thread " << this_thread::get_id() << " over, total so far: " << x << endl;
        };

    vector<thread> threads;
    for (int i = 0; i < 4; i++) {
        threads.emplace_back(counter);
    }
    for (int i = 0; i < 4; i++) {
        threads[i].join();
    }
    cout << "</naive counter>" << endl;
}

void guardedCounter() {
    cout << "<guarded counter>" << endl;
    static mutex logLock;
    static mutex countLock;
    auto counter = [](){
        logLock.lock();
        cout << "Thread " << this_thread::get_id() << " started" << endl;
        logLock.unlock();
        for (int i = 0; i < 2500; i++) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            countLock.lock();
            auto y = x;
            x = y + 1;
            countLock.unlock();
        }
        logLock.lock();
        cout << "Thread " << this_thread::get_id() << " over, total so far: " << x << endl;
        logLock.unlock();
        };

    vector<thread> threads;
    for (int i = 0; i < 4; i++) {
        threads.emplace_back(counter);
    }
    for (int i = 0; i < 4; i++) {
        threads[i].join();
    }
    cout << "</guarded counter>" << endl;
}

namespace philosophers {

void naiveSolution() {
    vector<thread> philosophers;
    const int noPhilosophers = 5000;
    static mutex forkLock[noPhilosophers];

    /*
     The problem: A number of philosophers sit at the table, between each of them a fork. They alternate between eating spaghett and thinking.
     The philosopher thinks for a bit, then attempts to eat - grabs both forks at his sides and eats for a bit. If one of the forks is unavailable,
     the philosopher can't start eating. 
     
     In the base problem they just go at it forever. Here I'll limit it 
    */

    int eatsLimit = 100;

    auto atTheTable = [eatsLimit, noPhilosophers](int threadId) {
        int eats = 0;
        int prevId = threadId == 0 ? noPhilosophers - 1 : threadId - 1;

        while (eats < eatsLimit) {
            std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 5 + 2));
            if (not forkLock[prevId].try_lock())
                continue;
            if (not forkLock[threadId].try_lock()) {
                forkLock[prevId].unlock();
                continue;
            }
            eats++;
            std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 5 + 3));
            forkLock[threadId].unlock();
            forkLock[prevId].unlock();
        }
        std::cout << threadId << " FINISHED" << endl; 
    };

    for (int i = 0; i < noPhilosophers; i++) {
        philosophers.emplace_back(atTheTable, i);
    }

    for (int i = 0; i < noPhilosophers; i++) {
        philosophers[i].join();
    }
}

}

namespace producerconsumer {

void onePrOneCo() {
    queue<int> stuff;
    const int maxSize = 10;
    condition_variable cvProduction;
    mutex m;

    auto consumer = [&stuff, &cvProduction](){
        int eaten = 0;
        cout << "Consumer start " << endl;

        while (eaten < 200) {
            std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 500 + 2));
    
            if (stuff.size() > 0)
            {
                eaten++;
                cout << "Consuming " << stuff.front() << endl;
                stuff.pop();
            }
            else;
                // we should go to sleep at this point
            if (stuff.size() == maxSize - 1)
                cvProduction.notify_one();
                // we should wake the producer at this point
        }
    };

    auto producer = [&stuff, &cvProduction, &m](){
        int made = 0;
        cout << "Producer start " << endl;

        while (made < 200) {
            std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 5 + 2));
            unique_lock<mutex> locko(m);

            if (stuff.size() < maxSize) {
                stuff.push(made++);
                cout << "Producing " << made << endl;
            }
            else {
                cout << "Cant produce " << made << endl;
                cvProduction.wait(locko);
                cout << "Wait done " << endl;
            }
                // we should go to sleep at this point
            if (stuff.size() == 1);
                // we should wake the consumer at this point
        }
    };

    
    thread c(consumer);
    thread p(producer);
    c.join();
    p.join();
    cout << "</naive counter>" << endl;
}

void multiProCo() {
    queue<int> stuff;
    const int maxSize = 10;
    condition_variable cvConsumption;
    condition_variable cvProduction;
    mutex m;
    mutex mP;
    mutex mC;

    int numOfConsumers = 3;
    int numOfProducers = 10;

    auto consumer = [&stuff, &cvConsumption, &cvProduction, &m](){
        int eaten = 0;  
        cout << "Consumer start " << endl;

        while (eaten < 200) {
            std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 200 + 2));
            unique_lock<mutex> locko(m);
    
            if (stuff.size() > 0)
            {
                eaten++;
                cout << "Consuming " << stuff.front() << endl;
                stuff.pop();
            }
            else
                cvConsumption.wait(locko);
            if (stuff.size() == maxSize - 1)
                cvProduction.notify_all();
        }
    };

    auto producer = [&stuff, &cvConsumption, &cvProduction, &m](int x){
        int made = x;
        cout << "Producer start " << endl;

        while (made < 100 + x) {
            std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 5 + 2));
            unique_lock<mutex> locko(m);

            if (stuff.size() < maxSize) {
                stuff.push(made++);
                cout << "Producing " << made << endl;
            }
            else {
                cout << "Cant produce " << made << endl;
                cvProduction.wait(locko);
                cout << "Wait done " << endl;
            }
            if (stuff.size() == 1);
                cvConsumption.notify_all();
        }
    };

    
    thread c(consumer);
    thread c2(consumer);
    thread p(producer, 0);
    thread p2(producer, 100);
    thread p3(producer, 200);
    thread p4(producer, 300);
    c.join();
    c2.join();
    p.join();
    p2.join();
    p3.join();
    p4.join();
    cout << "</naive counter>" << endl;
}
}

namespace sort {

void mergeSort(vector<int>& toSort) {
    function<void(vector<int>&, int, int, int)> merge = [](vector<int>& toMerge, int first, int midpoint, int last) {
        int iterLeft = first;
        int iterRight = midpoint + 1;
        vector<int> tmp;
        int i = 0;
        tmp.resize(last-first+1);

        while (iterLeft <= midpoint && iterRight <= last) {
            if (iterLeft > midpoint)
                tmp[i++] = toMerge[iterRight++];
            else if (iterRight > last)
                tmp[i++] = toMerge[iterLeft++];
            else if (toMerge[iterLeft] > toMerge[iterRight])
                tmp[i++] = toMerge[iterRight++];
            else
                tmp[i++] = toMerge[iterLeft++];
        }
        if (toMerge[iterLeft] < toMerge[iterRight])
            tmp[i++] = toMerge[iterRight++];
        else
            tmp[i++] = toMerge[iterLeft++];

        for (i = 0; i < tmp.size(); i++) {
            toMerge[first + i] = tmp[i];
        }
    };

    function<void(int,int)> sort = [&sort, &merge, &toSort](int first, int last) {
        int midpoint = (first + last) / 2;

        if (last == first)
            return;
        
        // sort(first, midpoint);
        // sort(midpoint + 1, last);
        thread left(sort, first, midpoint);
        thread right(sort, midpoint + 1, last);
        left.join();
        right.join();

        merge(toSort, first, midpoint, last);
    };

    sort(0, toSort.size() - 1);
}

}