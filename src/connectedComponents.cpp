void connectedComponents(
    vector<hoodtype>& components,
    const vector<hoodtype>& neighbourhoods,
    const hoodtype& subset,
    bool& isBipartite,
    hoodtype &left,
    hoodtype &right) {
    
    hoodtype seen;
    hoodtype seenRed;
    hoodtype seenBlue;
    isBipartite = true;

    subset.enumerate([
        &components, &neighbourhoods, &subset,
        &seen, &seenRed, &seenBlue,
        &isBipartite] (int i) {
        if (seen.test(i)) {
            return false;
        }
        deque<int> q;
        q.push_back(i);
        seen.set(i);
        seenRed.set(i);
        hoodtype current;

        while (!q.empty()) {
            const int top = q.front();
            q.pop_front();
            current.set(top);
            
            const bool isTopRed = seenRed.test(top);
            if (isTopRed && seenBlue.test(top)) {
                isBipartite = false;
            }

            const hoodtype nbs = (neighbourhoods[top] & subset);

            if (isTopRed) {
                seenBlue = seenBlue | nbs;
            } else {
                seenRed = seenRed | nbs;
            }
 
            // cerr << "NBS: ";
            // nbs.print(cerr);
            nbs.enumerate([
                &q, &top, &seen, 
                &seenRed, &seenBlue,
                &isBipartite] (int j) {

                // cerr << ", " << j;
                // if (j > 32) {
                //     throw "error";
                // }
                if (!seen.test(j)) {
                    q.push_back(j);
                    seen.set(j);
                    // if (seenRed.test(top)) {
                    //     if (seenRed.test(j)) {
                    //         isBipartite = false;
                    //     }
                    //     seenBlue.set(j);
                    // } else if (seenBlue.test(top)) {
                    //     if (seenBlue.test(j)) {
                    //         isBipartite = false;
                    //     }
                    //     seenRed.set(j);
                    // }
                // } else if (top != j) {
                //     if (seenRed.test(top) && seenRed.test(j)) {
                //         isBipartite = false;
                //     } else if (seenBlue.test(top) && seenBlue.test(j)) {
                //         isBipartite = false;
                //     }
                }
                return false;
            });

            // cerr << endl;
        }
        if (current != hoodtype()) {
            components.push_back(current);
        }

        return false;
    });

    isBipartite =
        isBipartite &&
        ((seenBlue | seenRed) == subset) &&
        ((seenBlue & seenRed) == hoodtype());

    if (isBipartite) {
        left = seenRed;
        right = seenBlue;
    }
}


