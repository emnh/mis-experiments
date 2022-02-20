
unsigned long long linearRankWidthHelper(const vector<int>& ordering, unsigned int i, const vector<hoodtype>& neighbourhoods) {
    hoodtype left;
    hoodtype right;
    vector<hoodtype> bipartite;
    vector<hoodtype> leftNeighbourhoods;
    vector<hoodtype> rightNeighbourhoods;

    for (unsigned int j = 0; j < ordering.size(); j++) {
        const int v = ordering[j];
        if (j <= i) {
            left.set(v);
        } else {
            right.set(v);
        }
        bipartite.push_back(hoodtype());
    }
    for (unsigned int j = 0; j < ordering.size(); j++) {
        const int v = ordering[j];
        if (left.test(v)) {
            bipartite[v] = neighbourhoods[v] & right;
            leftNeighbourhoods.push_back(bipartite[v]);
        } else {
            bipartite[v] = neighbourhoods[v] & left;
            rightNeighbourhoods.push_back(bipartite[v]);
        }
    }

    const unsigned long long cutrank =
        gf2_rank(leftNeighbourhoods.size() < rightNeighbourhoods.size() ? leftNeighbourhoods : rightNeighbourhoods);
    
    return cutrank;
}

unsigned long long linearRankWidth(vector<int> ordering, const vector<hoodtype>& neighbourhoods, unsigned int partiallyOrdered = 0, bool greedy = true) {
    unsigned long long ret = 0;
    // vector<int> maxOrdering;

    // for (unsigned int i = 0; i < ordering.size(); i++) {
    //     maxOrdering.push_back(ordering[i]);
    // }
    // sort(maxOrdering.begin(), maxOrdering.end(), 
    //     [&neighbourhoods](const int& a, const int& b) 
    //         { 
    //             return neighbourhoods[a].count() > neighbourhoods[b].count();
    //         });
    
    for (unsigned int i = 0; i < ordering.size(); i++) {
        if (greedy && i >= partiallyOrdered) {
            unsigned long long bestRank = 1ULL << 62;
            int best = i;
            for (unsigned int j = i + 1; j < ordering.size(); j++) {
                vector<int> newOrdering = ordering;
                const int tmp = newOrdering[i];
                ordering[i] = ordering[j];
                ordering[j] = tmp;

                const unsigned long long rank = linearRankWidthHelper(newOrdering, i, neighbourhoods);
                if (rank < bestRank) {
                    bestRank = rank;
                    best = j;
                }
            }

            // cerr << "swapping: " << i << " and " << best << endl;
            const int tmp = ordering[i];
            ordering[i] = ordering[best];
            ordering[best] = tmp;
        }

        auto start = myClock::now();
        unsigned long long cutrank = linearRankWidthHelper(ordering, i, neighbourhoods);
        const auto elapsed = std::chrono::duration_cast<time_interval_t>(myClock::now() - start);
        
        ret = max(ret, cutrank);
        // cerr << "i: " << i << " ret: " << ret << " in " << elapsed.count() / 1000 << "ms" << endl;
    }
    // cerr << "ordering: ";

    // vector<int> sortedOrdering = ordering;
    // sort(sortedOrdering.begin(), sortedOrdering.end(), 
    //     [&neighbourhoods](const int& a, const int& b) 
    //         { 
    //             return a < b;
    //         });
    
    // for (unsigned int i = 0; i < ordering.size(); i++) {
    //     assert(sortedOrdering[i] == i);
    //     cerr << ordering[i] << " ";
    // }
    // cerr << endl;
    return ret;
}