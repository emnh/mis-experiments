#include "prelude.cpp"
#include "countUnions.cpp"
#include "connectedComponents.cpp"
#include "ccmisProbe.cpp"
#include "ccmis.cpp"
#include "linearBoolWidth.cpp"
#include "gf2rank.cpp"
#include "treap.cpp"
#include "linearRankWidth.cpp"
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

int main(int argc, char* argv[]) {
    
    setStackSize();

    if (argc > 1 && strcmp(argv[1], "convert") == 0) {
        readGraph(true);
        return 0;
    }

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
    vector<int> minOrdering;
    for (int i = 0; i < neighbourhoods3.size(); i++) {
        P2.set(i);
        ordering.push_back(i);
        minOrdering.push_back(i);
    }
    sort(ordering.begin(), ordering.end(), 
            [&neighbourhoods3](const int& a, const int& b) 
                { 
                    return neighbourhoods3[a].count() > neighbourhoods3[b].count();
                });
    sort(minOrdering.begin(), minOrdering.end(), 
            [&neighbourhoods3](const int& a, const int& b) 
                { 
                    return neighbourhoods3[a].count() < neighbourhoods3[b].count();
                });
    // vector<int> ordering = defaultOrdering; // readOrdering();
    
    // cerr << "NEIGHBOURHOODS3: " << endl;
    // for (hoodtype h : neighbourhoods3) {
    //     h.print(cerr);
    //     cerr << endl;
    // }
    
    // auto start4 = myClock::now();
    // unsigned long long count4 = CCMIS(0, neighbourhoods3, P2, P2, X2, ordering);
    // const auto elapsed4 = std::chrono::duration_cast<time_interval_t>(myClock::now() - start4);
    // cout << "count4: " << count4 << " in " << elapsed4.count() / 1000 << endl;

    if (false) {
        auto start5 = myClock::now();
        uint64_t boolwidth = linearBoolWidth(minOrdering, neighbourhoods3);
        const auto elapsed5 = std::chrono::duration_cast<time_interval_t>(myClock::now() - start5);
        cout << "linear boolwidth UB: " << boolwidth << " in " << elapsed5.count() / 1000 << endl;
    }
   
    auto start6 = myClock::now();
    uint64_t rankwidth = linearRankWidth(ordering, neighbourhoods3);
    const auto elapsed6 = std::chrono::duration_cast<time_interval_t>(myClock::now() - start6);
    cout << "linear rankwidth UB: " << rankwidth << " in " << elapsed6.count() / 1000 << endl;

    auto start7 = myClock::now();
    uint64_t treaprankwidth = treapRankWidth(ordering, neighbourhoods3);
    const auto elapsed7 = std::chrono::duration_cast<time_interval_t>(myClock::now() - start7);
    cout << "treap rankwidth UB: " << treaprankwidth << " in " << elapsed7.count() / 1000 << endl;
    
    return 0;
}