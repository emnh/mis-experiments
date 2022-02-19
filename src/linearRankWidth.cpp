
unsigned long long linearRankWidth(vector<int> ordering, vector<hoodtype> neighbourhoods) {
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
        const unsigned long long cutrank =
            gf2_rank(leftNeighbourhoods.size() < rightNeighbourhoods.size() ? leftNeighbourhoods : rightNeighbourhoods);
        const auto elapsed = std::chrono::duration_cast<time_interval_t>(myClock::now() - start);

        ret = max(ret, cutrank);
        cerr << "i: " << i << " ret: " << ret << " in " << elapsed.count() / 1000 << "ms" << endl;
    }
    return ret;
}