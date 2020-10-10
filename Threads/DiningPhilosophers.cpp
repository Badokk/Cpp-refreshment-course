#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

namespace threads {
namespace philosophers {

using namespace std;

const int noPhilosophers = 5000;

void naiveSolution(int noEatingBreaks) {
    vector<thread> philosophers;
    static mutex forkLock[noPhilosophers];

    cout << "<dining philosophers>" << endl;

    auto atTheTable = [noEatingBreaks](int threadId) {
        int breaksTaken = 0;
        int prevId = threadId == 0 ? noPhilosophers - 1 : threadId - 1;

        while (breaksTaken < noEatingBreaks) {
            std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 5 + 2));
            if (not forkLock[prevId].try_lock())
                continue;
            if (not forkLock[threadId].try_lock()) {
                forkLock[prevId].unlock();
                continue;
            }
            breaksTaken++;
            std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 5 + 3));
            forkLock[threadId].unlock();
            forkLock[prevId].unlock();
        }
        cout << threadId << " finished" << endl; 
    };

    for (int i = 0; i < noPhilosophers; i++) {
        philosophers.emplace_back(atTheTable, i);
    }

    for (int i = 0; i < noPhilosophers; i++) {
        philosophers[i].join();
    }

    cout << "</dining philosophers>" << endl << endl;
}

}
}
