unsigned long long CCMISProbe(
    int depth, const vector<hoodtype>& neighbourhoods, const hoodtype& mask,
    const hoodtype& P, const hoodtype& X, vector<int> ordering) {
    if ((P | X) == hoodtype()) {
        return depth;
    }
    bool found = X.enumerate([&neighbourhoods, &mask, &P] (int i) {
        const hoodtype Xhood = neighbourhoods[i] & mask;
        if ((Xhood & P) == hoodtype()) {
            return true;
        }
        return false;
    });
    if (found) {
        return depth;
    }

    unsigned long long count = 0;

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
        int maxComponentSize = 0;
        int maxComponentIndex = -1;

        int i = 0;
        for (hoodtype component : components) {
            if (component.count() >= maxComponentSize) {
                maxComponentSize = component.count();
                maxComponentIndex = i; 
            }
            i++;
        }

        const hoodtype component = components[maxComponentIndex];
        const hoodtype newmask = mask & component;
        count =
            CCMISProbe(
                depth, neighbourhoods, newmask, 
                P & newmask, X & newmask, ordering);

        return count;
    }

    int v = ordering[depth];
    if (depth >= ordering.size()) {
        throw out_of_range("ordering");
    }
    for (int i = depth; i < ordering.size(); i++) {
        if (P.test(ordering[i])) {
            v = ordering[i];
            break;
        }
    }
    if (!(P).test(v)) {
        P.enumerate([&v](int i) {
            v = i;
            return true;
        });
        // throw out_of_range("v-ordering");
    }
    // int v = -1;
    // int maxd = 0;
    // P.enumerate([&neighbourhoods, &P, &mask, &maxd, &v] (int i) {
    //     const int d = (neighbourhoods[i]).count(); 
    //     if (maxd <= d) {
    //         maxd = d;
    //         v = i;
    //     }
    //     return false;
    // });

    if (rand() % 2 == 0) {
        hoodtype PminNv = P - (neighbourhoods[v] & mask);
        PminNv.reset(v);
        hoodtype XminNv = X - (neighbourhoods[v] & mask);
        count = CCMISProbe(depth + 1, neighbourhoods, mask, PminNv, XminNv, ordering);    
    } else {
        hoodtype Pminv = P;
        Pminv.reset(v);
        hoodtype Xuv = X;
        Xuv.set(v);
        count += CCMISProbe(depth + 1, neighbourhoods, mask, Pminv, Xuv, ordering);
    }
    
    return count;
}