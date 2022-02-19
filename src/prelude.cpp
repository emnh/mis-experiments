#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include <set>
#include <unordered_set>
#include <cmath>
#include <chrono>
#include <functional>
#include <deque>
#include <sys/resource.h>
#include <map>
#include <algorithm>
#include <cassert>

using namespace std;
using time_interval_t = std::chrono::microseconds;
using myClock = std::chrono::high_resolution_clock;

const bool DEBUG = true;

#include "bitset.cpp"

constexpr int N = 512;
// constexpr int N = 2048;
// constexpr int N = 512;
// constexpr int N = 128;
typedef BitSet<N> hoodtype;

struct node_hash {
    std::size_t operator()(const hoodtype& node) const {
        return std::hash<std::uint64_t>()(node.hash());
    }
};

void FisherYates(vector<int>& player, int from) { //implementation of Fisher
     int i, j, tmp; // create local variables to hold values for shuffle

     for (i = player.size() - 1; i > from; i--) { // for loop to shuffle
         j = rand() % (i + 1); //randomise j for shuffle with Fisher Yates
         tmp = player[j];
         player[j] = player[i];
         player[i] = tmp;
     }
}
