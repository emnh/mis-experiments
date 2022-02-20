
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
    
    for (unsigned int i = 0; i < ordering.size(); i++) {
        if (greedy && i >= partiallyOrdered) {
            unsigned long long bestRank = 1ULL << 62;
            int best = i;
            for (unsigned int j = i + 1; j < ordering.size(); j++) {
                vector<int> newOrdering = ordering;
                swap(newOrdering[i], newOrdering[j]);

                const unsigned long long rank = linearRankWidthHelper(newOrdering, i, neighbourhoods);
                if (rank < bestRank) {
                    bestRank = rank;
                    best = j;
                }
            }

            swap(ordering[i], ordering[best]);
        }

        auto start = myClock::now();
        unsigned long long cutrank = linearRankWidthHelper(ordering, i, neighbourhoods);
        const auto elapsed = std::chrono::duration_cast<time_interval_t>(myClock::now() - start);
        
        ret = max(ret, cutrank);
    }
    return ret;
}

unsigned long long treapRankWidthHelper(hoodtype all, const vector<int>& leftSide, const vector<hoodtype>& neighbourhoods) {
    hoodtype left;
    hoodtype right;
    vector<hoodtype> bipartite;
    vector<hoodtype> leftNeighbourhoods;
    vector<hoodtype> rightNeighbourhoods;

    for (unsigned int j = 0; j < leftSide.size(); j++) {
        left.set(leftSide[j]);
    }
    for (unsigned int j = 0; j < neighbourhoods.size(); j++) {
        bipartite.push_back(hoodtype());
    }
    right = all - left;
    for (unsigned int j = 0; j < neighbourhoods.size(); j++) {
        const unsigned int v = j;
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


unsigned long long treapRankWidth(vector<int> ordering, const vector<hoodtype>& neighbourhoods, unsigned int partiallyOrdered = 0, bool greedy = true) {
    unsigned long long ret = 0;

    Treap treap;
    hoodtype all;
    for (unsigned int i = 0; i < ordering.size(); i++) {
        all.set(ordering[i]);
    }

    for (unsigned int i = 0; i < ordering.size(); i++) {
        if (greedy && i >= partiallyOrdered) {
            unsigned long long bestRank = 1ULL << 62;
            int best = i;
            for (unsigned int j = i + 1; j < ordering.size(); j++) {
                Treap newTreap = treap;
                newTreap.insertNode(j, ordering[j]);

                const vector<vector<int>> bags = newTreap.getAllBags();
                unsigned long long maxRank = 0;
                for (unsigned int k = 0; k < bags.size(); k++) {
                    const unsigned long long rank = treapRankWidthHelper(all, bags[k], neighbourhoods);
                    maxRank = max(maxRank, rank);
                }
                if (maxRank < bestRank) {
                    bestRank = maxRank;
                    best = j;
                }
            }

            swap(ordering[i], ordering[best]);
        }

        treap.insertNode(i, ordering[i]);

        auto start = myClock::now();
        const vector<vector<int>> bags = treap.getAllBags();
        unsigned long long maxRank = 0;
        for (unsigned int k = 0; k < bags.size(); k++) {
            const unsigned long long rank = treapRankWidthHelper(all, bags[k], neighbourhoods);
            maxRank = max(maxRank, rank);
        }
        const auto elapsed = std::chrono::duration_cast<time_interval_t>(myClock::now() - start);
        
        ret = maxRank;
        // ret = max(ret, cutrank);
    }

    return ret;
}