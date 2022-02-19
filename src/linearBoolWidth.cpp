
unsigned long long linearBoolWidth(vector<int> ordering, vector<hoodtype> neighbourhoods) {
    unsigned long long ret = 0;
    vector<int> maxOrdering;

    for (int i = 0; i < ordering.size(); i++) {
        maxOrdering.push_back(ordering[i]);
    }
    sort(maxOrdering.begin(), maxOrdering.end(), 
        [&neighbourhoods](const int& a, const int& b) 
            { 
                return neighbourhoods[a].count() > neighbourhoods[b].count();
            });
    
    for (int i = 0; i < ordering.size(); i++) {
        hoodtype left;
        hoodtype right;
        vector<hoodtype> bipartite;
        vector<hoodtype> leftNeighbourhoods;
        vector<hoodtype> rightNeighbourhoods;
        for (int j = 0; j < ordering.size(); j++) {
            const int v = ordering[j];
            if (j <= i) {
                left.set(v);
            } else {
                right.set(v);
            }
            bipartite.push_back(hoodtype());
        }
        for (int j = 0; j < ordering.size(); j++) {
            const int v = ordering[j];
            if (left.test(v)) {
                bipartite[v] = neighbourhoods[v] & right;
                leftNeighbourhoods.push_back(bipartite[v]);
            } else {
                bipartite[v] = neighbourhoods[v] & left;
                rightNeighbourhoods.push_back(bipartite[v]);
            }
        }
        const hoodtype P = left | right;
        const hoodtype X;

        auto start = myClock::now();
        const unsigned long long cutbool =
            ret < 1000000 ?
                countUnions2(leftNeighbourhoods.size() < rightNeighbourhoods.size() ? leftNeighbourhoods : rightNeighbourhoods) :
                CCMIS(0, bipartite, P, P, X, maxOrdering);
        const auto elapsed = std::chrono::duration_cast<time_interval_t>(myClock::now() - start);

        auto start2 = myClock::now();       
        const unsigned long long cutbool2 = CCMIS(0, bipartite, P, P, X, maxOrdering);
        const auto elapsed2 = std::chrono::duration_cast<time_interval_t>(myClock::now() - start2);

        cerr << "cutbools: " << cutbool << " in " << elapsed.count() / 1000 << "ms and " << cutbool2 << " in " << elapsed2.count() / 1000 << "ms" << endl;
        ret = max(ret, cutbool);
        cerr << "i: " << i << " ret: " << ret << endl;
    }
    return ret;
}