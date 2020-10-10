#include <thread>
#include <vector>

namespace threads {
namespace sort {

using namespace std;

void merge(vector<int>& toMerge, int first, int midpoint, int last) {
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
        else
            tmp[i++] = toMerge[iterLeft] < toMerge[iterRight]
                ? toMerge[iterLeft++]
                : toMerge[iterRight++];
    }

    tmp[i++] = toMerge[iterLeft] < toMerge[iterRight]
        ? toMerge[iterRight]
        : toMerge[iterLeft];

    for (i = 0; i < tmp.size(); i++) {
        toMerge[first + i] = tmp[i];
    }
}

void sort(vector<int>& toSort, int firstIndex, int lastIndex) {
        int midpoint = (firstIndex + lastIndex) / 2;

        if (lastIndex == firstIndex)
            return;

        thread left(sort, ref(toSort), firstIndex, midpoint);
        thread right(sort, ref(toSort), midpoint + 1, lastIndex);
        left.join();
        right.join();

        merge(toSort, firstIndex, midpoint, lastIndex);
    };

void mergeSort(vector<int>& toSort) {
    sort(toSort, 0, toSort.size() - 1);
}

}
}