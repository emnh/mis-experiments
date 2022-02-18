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

using namespace std;
using time_interval_t = std::chrono::microseconds;
using myClock = std::chrono::high_resolution_clock;

const bool DEBUG = true;

#include "bitset.cpp"

constexpr int N = 512;
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

#include "countUnions.cpp"
#include "connectedComponents.cpp"
#include "ccmisProbe.cpp"
#include "ccmis.cpp"
#include "readGraph.cpp"
#include "readOrdering.cpp"

void setStackSize() {
    const rlim_t kStackSize = 1024 * 1024 * 1024;   // min stack size = 1024 MB
    struct rlimit rl;
    int result;

    result = getrlimit(RLIMIT_STACK, &rl);
    if (result == 0)
    {
        if (rl.rlim_cur < kStackSize)
        {
            rl.rlim_cur = kStackSize;
            result = setrlimit(RLIMIT_STACK, &rl);
            if (result != 0)
            {
                fprintf(stderr, "setrlimit returned result = %d\n", result);
            }
        }
    }
}

int main() {
    
    setStackSize();

    vector<hoodtype> neighbourhoods;
    const int maxi = 20;
    for (int i = 0; i < maxi; i++) {
        hoodtype hood;
        hood.set(i);
        hood.set((i + 1) % maxi);
        // if ((i + 1) % 3 == 0) {
        //     X.set(i);
        // }
        neighbourhoods.push_back(hood);
    }

    // cerr << "NEIGHBOURHOODS: " << endl;
    // for (hoodtype h : neighbourhoods) {
    //     h.print(cerr);
    //     cerr << endl;
    // }

    auto start = myClock::now();
    unsigned long long count = countUnions(neighbourhoods);
    const auto elapsed = std::chrono::duration_cast<time_interval_t>(myClock::now() - start);
    cout << "count1: " << count << " in " << elapsed.count() / 1000 << endl;
    
    auto start2 = myClock::now();
    unsigned long long count2 = countUnions2(neighbourhoods);
    const auto elapsed2 = std::chrono::duration_cast<time_interval_t>(myClock::now() - start2);
    cout << "count2: " << count2 << " in " << elapsed2.count() / 1000 << endl;
    
    // Convert list of neighbourhoods in bipartite graph to a graph 
    vector<hoodtype> neighbourhoods2;
    hoodtype P = hoodtype();
    hoodtype X = hoodtype();
    for (int i = 0; i < maxi * 2; i++) {
        P.set(i);
        neighbourhoods2.push_back(hoodtype());
    }
    for (int i = 0; i < maxi; i++) {
        const hoodtype hood = neighbourhoods[i];
        for (int j = 0; j < maxi; j++) {
            if (hood.test(j)) {
                neighbourhoods2[i + maxi].set(j);
                neighbourhoods2[j].set(i + maxi);
            }
        }
    }
    // cerr << "NEIGHBOURHOODS2: " << endl;
    // for (hoodtype h : neighbourhoods2) {
    //     h.print(cerr);
    //     cerr << endl;
    // }
    
    auto start3 = myClock::now();
    vector<int> defaultOrdering;
    for (int i = 0; i < maxi * 2; i++) {
        defaultOrdering.push_back(i);
    }
    unsigned long long count3 = CCMIS(0, neighbourhoods2, P, P, X, defaultOrdering);
    const auto elapsed3 = std::chrono::duration_cast<time_interval_t>(myClock::now() - start3);
    cout << "count3: " << count3 << " in " << elapsed3.count() / 1000 << endl;

    const vector<hoodtype> neighbourhoods3 = readGraph();
    hoodtype P2 = hoodtype();
    hoodtype X2 = hoodtype();
    vector<int> ordering;
    for (int i = 0; i < neighbourhoods3.size(); i++) {
        P2.set(i);
        ordering.push_back(i);
    }
    sort(ordering.begin(), ordering.end(), 
            [&neighbourhoods3](const int& a, const int& b) 
                { 
                    return neighbourhoods3[a].count() > neighbourhoods3[b].count();
                });
    // vector<int> ordering = defaultOrdering; // readOrdering();
    
    // cerr << "NEIGHBOURHOODS3: " << endl;
    // for (hoodtype h : neighbourhoods3) {
    //     h.print(cerr);
    //     cerr << endl;
    // }
    
    auto start4 = myClock::now();
    unsigned long long count4 = CCMIS(0, neighbourhoods3, P2, P2, X2, ordering);
    const auto elapsed4 = std::chrono::duration_cast<time_interval_t>(myClock::now() - start4);
    cout << "count4: " << count4 << " in " << elapsed4.count() / 1000 << endl;

    return 0;
}