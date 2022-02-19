unsigned long long countUnions(const vector<hoodtype>& neighbourhoods) {
    unordered_set<hoodtype, node_hash> s1(1 << neighbourhoods.size());
    vector<hoodtype> s2;
    s1.insert(hoodtype());
    for (hoodtype hood : neighbourhoods) {
        vector<hoodtype> s2(s1.begin(), s1.end()); 
        for (hoodtype hood2 : s2) {
            s1.insert(hood | hood2);
        }
    }
    // if (DEBUG) {
    //     for (hoodtype hood : s1) {
    //         hood.print(cerr);
    //         cerr << endl;
    //     }
    // }
    
    // cerr << "HOODS: " << endl;
    // for (hoodtype h : s1) {
    //     h.print(cerr);
    //     cerr << endl;
    // }

    return s1.size();
}

unsigned long long countUnions2(const vector<hoodtype>& neighbourhoods) {
    unordered_set<hoodtype, node_hash> s1(1 << min(16, (int) neighbourhoods.size()));
    // unordered_set<hoodtype, node_hash> s1(256);
    vector<hoodtype> vs[neighbourhoods.size() + 1];
    // for (int i = 0; i <= neighbourhoods.size(); i++) {
    //     vs[i] = hoodtype(); // vs.push_back(vector<hoodtype>());
    // }
    s1.insert(hoodtype());
    vs[0].push_back(hoodtype());
    for (int i = 0; i < neighbourhoods.size(); i++) {
        hoodtype current = neighbourhoods[i];
        // vs[i + 1].reserve(s1.size() / 2);
        for (int j = 0; j <= i; j++) {
            for (hoodtype hood : vs[j]) {
                hoodtype next = current | hood;
                if (s1.find(next) == s1.end()) {
                    s1.insert(next);
                    vs[i + 1].push_back(next);
                }
            }
        }
    }
    // cerr << "HOODS: " << endl;
    // for (hoodtype h : s1) {
    //     h.print(cerr);
    //     cerr << endl;
    // }
    return s1.size();
}

unsigned long long countUnionsWithMask(
    vector<hoodtype>& neighbourhoods,
    const hoodtype& exclusionMask) {
    unordered_set<hoodtype, node_hash> s1(1 << neighbourhoods.size());
    // unordered_set<hoodtype, node_hash> s1(256);
    vector<hoodtype> vs[neighbourhoods.size() + 1];
    // for (int i = 0; i <= neighbourhoods.size(); i++) {
    //     vs[i] = hoodtype(); // vs.push_back(vector<hoodtype>());
    // }
    s1.insert(hoodtype());
    vs[0].push_back(hoodtype());
    for (int i = 0; i < neighbourhoods.size(); i++) {
        hoodtype current = neighbourhoods[i];
        // vs[i + 1].reserve(s1.size() / 2);
        for (int j = 0; j <= i; j++) {
            for (hoodtype hood : vs[j]) {
                hoodtype next = (current | hood) - exclusionMask;
                if (s1.find(next) == s1.end()) {
                    s1.insert(next);
                    vs[i + 1].push_back(next);
                }
            }
        }
    }
    // cerr << "HOODS: " << endl;
    // for (hoodtype h : s1) {
    //     h.print(cerr);
    //     cerr << endl;
    // }
    return s1.size();
}