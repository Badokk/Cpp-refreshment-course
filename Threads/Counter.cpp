#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

namespace threads {
namespace counter {

using namespace std;

void naiveCounter(int noThreads) {
    cout << "<naive counter>" << endl;
    static int x = 0;

    auto counter = [](int threadId){
        cout << "Thread " << threadId << " started" << endl;
        for (int i = 0; i < 2500; i++) {
            this_thread::sleep_for(chrono::milliseconds(1));
            auto y = x;
            x = y + 1;
        }
        cout << "Thread " << threadId << " over, total so far: " << x << endl;
        };

    vector<thread> threads;
    for (int i = 0; i < noThreads; i++) {
        threads.emplace_back(counter, i);
    }
    for (int i = 0; i < noThreads; i++) {
        threads[i].join();
    }
    cout << "Result: " << x << endl;
    cout << "</naive counter>" << endl;
}

void guardedCounter(int noThreads) {
    cout << "<guarded counter>" << endl;
    static mutex logLock;
    static mutex countLock;
    static int x = 0;

    auto counter = [](int threadId){
        logLock.lock();
        cout << "Thread " << threadId << " started" << endl;
        logLock.unlock();
        for (int i = 0; i < 2500; i++) {
            this_thread::sleep_for(chrono::milliseconds(1));
            countLock.lock();
            auto y = x;
            x = y + 1;
            countLock.unlock();
        }
        logLock.lock();
        cout << "Thread " << threadId << " over, total so far: " << x << endl;
        logLock.unlock();
        };

    vector<thread> threads;
    for (int i = 0; i < noThreads; i++) {
        threads.emplace_back(counter, i);
    }
    for (int i = 0; i < noThreads; i++) {
        threads[i].join();
    }
    cout << "Result: " << x << endl;
    cout << "</guarded counter>" << endl;
}

}
}