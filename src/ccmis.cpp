unsigned long long CCMIS(
    const int depth, const vector<hoodtype>& neighbourhoods, const hoodtype& mask,
    const hoodtype& P, const hoodtype& X, const vector<int>& ordering) {
    // for (int i = 0; i < depth; i++) {
    //     cerr << "--";
    // }
    // cerr << "P: ";
    // P.print(cerr);
    // cerr << " X: ";
    // X.print(cerr);
    // cerr << endl;

    if ((P | X) == hoodtype()) {
        // cerr << "return 1" << endl;
        return 1;
    }
    // if (P == hoodtype() && X != hoodtype()) {
    //     // cerr << "return 0" << endl;
    //     return 0;
    // }
    bool found = X.enumerate([&neighbourhoods, &mask, &P] (int i) {
        const hoodtype Xhood = neighbourhoods[i] & mask;
        // if ((Xhood - P) == Xhood) {
        if ((Xhood & P) == hoodtype()) {
            // cerr << "w in X: " << i << " hood: ";
            // Xhood.print(cerr);
            // cerr << " P: ";
            // P.print(cerr);
            // cerr << endl;
            return true;
        }
        return false;
    });
    if (found) {
        // cerr << "return 0" << endl;
        return 0;
    }

    unsigned long long count = 0;

    int v = ordering[depth];
    // if (depth >= ordering.size()) {
    //     throw out_of_range("ordering");
    // }
    // for (int i = depth; i < ordering.size(); i++) {
    //     if (P.test(ordering[i])) {
    //         v = ordering[i];
    //         break;
    //     }
    // }
    // if (!(P).test(v)) { 
    //     P.enumerate([&v](int i) {
    //         v = i;
    //         return true;
    //     });
    //     // throw out_of_range("v-ordering");
    // }

    int maxd = 0;
    P.enumerate([&neighbourhoods, &mask, &maxd, &v] (int i) {
        const int d = (neighbourhoods[i] & mask).count(); 
        if (maxd <= d) {
            maxd = d;
            v = i;
        }
        return false;
    });
    // if (!P.test(v)) {
    //     throw out_of_range("v-ordering");
    // }

    // if (P.count() >= 32 && depth < 0) {
    //     int maxd = 0;
    //     vector<int> Pset;
    //     for (int i = 0; i < depth; i++) {
    //         Pset.push_back(ordering[i]);
    //     }
    //     P.enumerate([&neighbourhoods, &Pset, &P, &mask, &maxd, &v] (int i) {
    //         const int d = (neighbourhoods[i]).count(); 
    //         // const int d = (neighbourhoods[i]).count(); 
    //         if (maxd <= d) {
    //             maxd = d;
    //             v = i;
    //         }
    //         Pset.push_back(i);
    //         return false;
    //     });
    //     sort(Pset.begin(), Pset.end(), 
    //         [&neighbourhoods](const int& a, const int& b) 
    //             { 
    //                 return neighbourhoods[a].count() > neighbourhoods[b].count();
    //             });
        
    //     int mindepth = 0;
    //     int minv = v;
    //     // vector<int> minOrdering = Pset;
    //     vector<int> minOrdering = ordering;

    //     const int maxj = 20;

    //     for (int j = 0; j < maxj; j++) {
    //         mindepth += CCMISProbe(depth, neighbourhoods, mask, P, X, minOrdering);
    //     }

    //     for (int i = 0; i < Pset.size(); i++) {
            
    //         // const int v2 = Pset[i];

    //         vector<int> newOrdering =
    //             vector<int>(ordering.begin(), ordering.end());
    //         FisherYates(newOrdering, depth);

    //         int d = 0;
    //         for (int j = 0; j < maxj; j++) {
    //             d += CCMISProbe(depth, neighbourhoods, mask, P, X, newOrdering);
    //         }
                
    //         if (d <= mindepth) {
    //             mindepth = d;
    //             // minv = v2;
    //             minOrdering = newOrdering;
    //         }
    //     }
    //     ordering = minOrdering;
    //     v = ordering[depth];
    //     for (int i = depth; i < ordering.size(); i++) {
    //         if (P.test(ordering[i])) {
    //             v = ordering[i];
    //             break;
    //         }
    //     }
    //     if (!(P).test(v)) {
    //         P.enumerate([&v](int i) {
    //             v = i;
    //             return true;
    //         });
    //         // throw out_of_range("v-ordering");
    //     }
    // }

    if (P.count() >= 10) {
        vector<hoodtype> components;
        bool isBipartite = false;
        hoodtype left;
        hoodtype right;
        // TODO: Only check if it is connected if we removed a node
        connectedComponents(
            components,
            neighbourhoods,
            (P | X) & mask,
            isBipartite,
            left,
            right);

        if (components.size() > 1) {

            count = 1;
            for (hoodtype component : components) {
                const hoodtype newmask = mask & component;
                count *= CCMIS(depth, neighbourhoods, newmask, P & newmask, X & newmask, ordering);
            }

            return count;
        }

        const bool countBipartite = false;
        if (countBipartite && isBipartite &&
            // max(left.count(), right.count()) >= 4 && 
            min(left.count(), right.count()) <= 20) { 
            // ((X - left) == hoodtype() ||
            //  (X - right) == hoodtype())) {
            vector<hoodtype> neighbourhoodsOfMin;
            const hoodtype& minset = left.count() < right.count() ? left : right;
            const hoodtype& maxset = left.count() < right.count() ? right : left;
            // const hoodtype& minset = (X - left) == hoodtype() ? left : right;
            // const hoodtype& maxset = (X - left) == hoodtype() ? right : left;
            hoodtype NX;
            X.enumerate([&neighbourhoods, &mask, &NX] (int i) {
                NX = NX | (neighbourhoods[i] & mask); 
                return false;
            });
            ((minset - (X | NX)) & mask).enumerate([
                &neighbourhoods, &mask, &X, &NX,
                &neighbourhoodsOfMin, &maxset] (int i) {
                neighbourhoodsOfMin.push_back(
                    ((((neighbourhoods[i] | (X | NX)) & maxset)) & mask));
                return false;
            });
            return countUnions2(neighbourhoodsOfMin);
            // return countUnionsWithMask(neighbourhoodsOfMin, X);
            // cerr << "bipartite: ";
            // left.print(cerr);
            // cerr << " ";
            // right.print(cerr);
            // cerr << endl;
        }
    }

    

    // int minIndex = 10000;
    // P.enumerate([&neighbourhoods, &minIndex, &ordering, &v] (int i) {
    //     int j = 0;
    //     // TODO: optimize
    //     int d = ordering[i];
    //     if (d < minIndex) {
    //         minIndex = d;
    //         v = i;
    //     }
    //     // for (int n : ordering) {
    //     //     // cerr << "N: " << n << " I: " << i << " J: " << j << " V: " << v << " MI: " << minIndex << endl;
    //     //     if (n == i && j < minIndex) {
    //     //         minIndex = j;
    //     //         v = i;
    //     //         break;
    //     //     }
    //     //     j++;
    //     // }
    //     return false;
    // });

    if (v == -1) {
        // cerr << "return 0" << endl;
        throw "error: " + v;
    }
    // cerr << "V: " << v << " VHOOD: ";
    // (neighbourhoods[v] & mask).print(cerr);
    // cerr << endl;

    // const bool smallUnionsBruteForce = false;
    // if (smallUnionsBruteForce && P.count() <= 8) {
    //     count = 0;
    //     const int subsets = 1 << P.count();
    //     for (uint64_t i = 0; i < subsets; i++) {
    //         hoodtype subset;
    //         hoodtype nbs;
    //         hoodtype Xnew = X;
    //         int seq = 0;
    //         P.enumerate([
    //             &neighbourhoods, &mask,
    //             &subset, &nbs, &Xnew,
    //             &seq, &i] (int j) {
    //             if (i & (1ULL << seq)) {
    //                 subset.set(j);
    //                 nbs = nbs | (neighbourhoods[j] & mask);
    //             } else {
    //                 Xnew.set(j);
    //             }
    //             seq++;
    //             return false;
    //         });
    //         if ((subset & nbs) == hoodtype() &&
    //             (subset & Xnew) == hoodtype() &&
    //             (Xnew - nbs) == hoodtype()) {
    //             count++;
    //         }
    //     }
    //     return count;
    // }

    {
        hoodtype PminNv = P - (neighbourhoods[v] & mask);
        PminNv.reset(v);
        hoodtype XminNv = X - (neighbourhoods[v] & mask);
        count = CCMIS(depth + 1, neighbourhoods, mask, PminNv, XminNv, ordering);
        hoodtype Pminv = P;
        Pminv.reset(v);
        hoodtype Xuv = X;
        Xuv.set(v);
        count += CCMIS(depth + 1, neighbourhoods, mask, Pminv, Xuv, ordering);
    }
    
    return count;
}