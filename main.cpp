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

using namespace std;
using time_interval_t = std::chrono::microseconds;
using myClock = std::chrono::high_resolution_clock;

const bool DEBUG = true;

template<std::size_t N>
class BitSet {
    static constexpr int Nlong = ceil((float) N / 64);
    uint64_t data[Nlong];
public:
    BitSet() {
        // cerr << Nlong << endl;
        for (int i = 0; i < Nlong; i++) {
            data[i] = 0ULL;
        }
    }

    void set(int i) {
        data[i / 64] |= (1ULL << (i % 64));
    }

    void reset(int i) {
        data[i / 64] &= ~(1ULL << (i % 64));
    }

    const bool test(int i) const {
        // cerr << "I: " << i << endl;
        return data[i / 64] & (1ULL << (i % 64));
    }

    void flip(int i) {
        data[i / 64] ^= (1ULL << (i % 64));
    }

    const int count() const {
        int cnt = 0;
        for (int i = 0; i < Nlong; i++) {
            cnt += __builtin_popcountll(data[i]);
        }
        return cnt;
    }

    void print(std::ostream &os) const {
        for (int i = Nlong - 1; i >= 0; i--) {
            os << bitset<64>(data[i]);
        }
    }

    const BitSet<N> operator- (const BitSet<N>& rhs) const noexcept {
        BitSet<N> ret;
        for (int i = 0; i < Nlong; i++) {
            ret.data[i] = data[i] & (~rhs.data[i]);
        }
        return ret;
    }

    const BitSet<N> operator& (const BitSet<N>& rhs) const noexcept {
        BitSet<N> ret;
        for (int i = 0; i < Nlong; i++) {
            ret.data[i] = data[i] & rhs.data[i];
        }
        return ret;
    }

    const BitSet<N> operator| (const BitSet<N>& rhs) const noexcept {
        BitSet<N> ret;
        for (int i = 0; i < Nlong; i++) {
            ret.data[i] = data[i] | rhs.data[i];
        }
        return ret;
    }

    const BitSet<N> operator^ (const BitSet<N>& rhs) const noexcept {
        BitSet<N> ret;
        for (int i = 0; i < Nlong; i++) {
            ret.data[i] = data[i] ^ rhs.data[i];
        }
        return ret;
    }

    const uint64_t hash() const noexcept {
        uint64_t ret = 0;
        for (int i = 0; i < Nlong; i++) {
            ret = ret ^ data[i];
        }
        return ret;
    }

    const int operator< (const BitSet<N>& rhs) const noexcept {
        for (int i = 0; i < Nlong; i++) {
            if (data[i] ^ rhs.data[i]) {
                return data[i] < rhs.data[i];
            }
        }
        return 0;
    }

    const int operator== (const BitSet<N>& rhs) const noexcept {
        for (int i = 0; i < Nlong; i++) {
            if (data[i] != rhs.data[i]) {
                return false;
            }
        }
        return true;
    }

    
    const int operator!= (const BitSet<N>& rhs) const noexcept {
        return !(*this == rhs);
    }

    const bool enumerate(function<bool(int)> f) const {
        for (int i = 0; i < Nlong; i++) {
            uint64_t tmpX = data[i];
            int j = 0;
            while (tmpX != 0) {
                const int setbit = __builtin_ctzll(tmpX);
                j += setbit;
                // if ((j + i * 64) >= Nlong * 64) {
                //     cerr << "JI: " << j << " " << i << " " << j + i * 64 << " setbit " << setbit << endl;
                //     throw new out_of_range("error");
                // }
                if (f(j + i * 64)) {
                    return true;
                }
                j++;
                tmpX >>= setbit;
                tmpX >>= 1;
            }
        }
        return false;
    }
};

// constexpr int N = 512;
constexpr int N = 64;
typedef BitSet<N> hoodtype;

struct node_hash {
    std::size_t operator()(const hoodtype& node) const {
        return std::hash<std::uint64_t>()(node.hash());
    }
};

unsigned long long countUnions(vector<hoodtype>& neighbourhoods) {
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

unsigned long long countUnions2(vector<hoodtype>& neighbourhoods) {
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

void connectedComponents(vector<hoodtype>& components, const vector<hoodtype>& neighbourhoods, const hoodtype& subset) {
    hoodtype seen;

    subset.enumerate([&components, &neighbourhoods, &subset, &seen] (int i) {
        if (seen.test(i)) {
            return false;
        }
        deque<int> q;
        q.push_back(i);
        seen.set(i);
        hoodtype current;

        while (!q.empty()) {
            const int top = q.front();
            q.pop_front();
            current.set(top);
            
            const hoodtype nbs = (neighbourhoods[top] & subset);
            // cerr << "NBS: ";
            // nbs.print(cerr);
            nbs.enumerate([&q, &seen] (int j) {

                // cerr << ", " << j;
                // if (j > 32) {
                //     throw "error";
                // }
                if (!seen.test(j)) {
                    q.push_back(j);
                    seen.set(j);
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
}

unsigned long long CCMIS(int depth, const vector<hoodtype>& neighbourhoods, const hoodtype& mask, const hoodtype& P, const hoodtype& X, map<int, int> ordering) {
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

    vector<hoodtype> components;
    connectedComponents(components, neighbourhoods, (P | X) & mask);
    if (components.size() > 1) {

        count = 1;
        for (hoodtype component : components) {
            const hoodtype newmask = mask & component;
            count *= CCMIS(depth + 1, neighbourhoods, newmask, P & newmask, X & newmask, ordering);
        }

        return count;
    }

    int v = -1;
    int maxd = 0;
    P.enumerate([&neighbourhoods, &maxd, &v] (int i) {
        if (maxd <= neighbourhoods[i].count()) {
            maxd = neighbourhoods[i].count();
            v = i;
        }
        return false;
    });

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

    const bool smallUnionsBruteForce = true;
    if (smallUnionsBruteForce && P.count() <= 8) {
        count = 0;
        const int subsets = 1 << P.count();
        for (uint64_t i = 0; i < subsets; i++) {
            hoodtype subset;
            hoodtype nbs;
            hoodtype Xnew = X;
            int seq = 0;
            P.enumerate([
                &neighbourhoods, &mask,
                &subset, &nbs, &Xnew,
                &seq, &i] (int j) {
                if (i & (1ULL << seq)) {
                    subset.set(j);
                    nbs = nbs | (neighbourhoods[j] & mask);
                } else {
                    Xnew.set(j);
                }
                seq++;
                return false;
            });
            if ((subset & nbs) == hoodtype() &&
                (subset & Xnew) == hoodtype() &&
                (Xnew - nbs) == hoodtype()) {
                count++;
            }
        }
        return count;
    }

    hoodtype PminNv = P - (neighbourhoods[v] & mask);
    PminNv.reset(v);
    hoodtype XminNv = X - (neighbourhoods[v] & mask);
    count = CCMIS(depth + 1, neighbourhoods, mask, PminNv, XminNv, ordering);
    hoodtype Pminv = P;
    Pminv.reset(v);
    hoodtype Xuv = X;
    Xuv.set(v);
    count += CCMIS(depth + 1, neighbourhoods, mask, Pminv, Xuv, ordering);

    return count;
}

vector<hoodtype> readGraph() {
    // string a, b;
    // int c, d;
    // cin << a << b << c << d;
    // cin.ignore();
    // cin << a << b << c << d;
    
    vector<hoodtype> neighbourhoods;
    
    string s;

    int edges = 0;

    while (!cin.eof()) {
        cin >> s;
        if (s == "n") {
            // neighbourhoods.push_back(hoodtype());
        } else if (s == "e") {
            int a, b;
            cin >> a >> b;
            if (neighbourhoods.size() <= max(a, b) + 2) {
                neighbourhoods.resize(max(a, b) + 2);
            }
            // edges++;
            neighbourhoods[a].set(b);
            // cerr << a << " " << b << endl;
            neighbourhoods[b].set(a);
        }
        getline(cin, s);
    }

    for (hoodtype hood : neighbourhoods) {
        edges += hood.count();
    }

    int node = 0;
    int realEdges = 0;
    ofstream myfile;
    myfile.open("graph.txt");
    myfile << neighbourhoods.size() << " " << edges / 2 << endl; 
    for (hoodtype hood : neighbourhoods) {
        node++;
        // if (hood.count() > 0) {
        // myfile << node;
        bool first = true;
        hood.enumerate([&myfile, &realEdges, &first] (int i) {
            if (!first) {
                myfile << " ";
            }
            first = false;
            myfile << (i + 1);
            realEdges++;
            return false;
        });
        myfile << endl;
        // }
    }
    myfile.close();

    int i = system("ndmetis -ccorder graph.txt");
    // cerr << realEdges << endl;

    return neighbourhoods;
}

map<int, int> readOrdering() {
    vector<int> ordering;
    ifstream ndmetisFile;
    ndmetisFile.open("graph.txt.iperm");
    while (!ndmetisFile.eof()) {
        int n;
        ndmetisFile >> n;
        ndmetisFile.ignore();
        ordering.push_back(n);
    }
    ordering.pop_back();
    ndmetisFile.close();
    cerr << "METIS: " << endl;
    map<int, int> orderingMap;
    for (int i = 0; i < ordering.size(); i++) {
        cerr << ordering[i] << " ";
        // orderingMap[ordering.size() - 1 - ordering[i]] = i;
        orderingMap[ordering.size() - ordering[i] - 1] = i;
    }
    cerr << endl;
    return orderingMap;
}

int main() {
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

    cerr << "NEIGHBOURHOODS: " << endl;
    for (hoodtype h : neighbourhoods) {
        h.print(cerr);
        cerr << endl;
    }

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
    map<int, int> defaultOrdering;
    for (int i = 0; i < maxi * 2; i++) {
        defaultOrdering[i] = i;
    }
    unsigned long long count3 = CCMIS(0, neighbourhoods2, P, P, X, defaultOrdering);
    const auto elapsed3 = std::chrono::duration_cast<time_interval_t>(myClock::now() - start3);
    cout << "count3: " << count3 << " in " << elapsed3.count() / 1000 << endl;

    const vector<hoodtype> neighbourhoods3 = readGraph();
    hoodtype P2 = hoodtype();
    hoodtype X2 = hoodtype();
    for (int i = 0; i < neighbourhoods3.size(); i++) {
        P2.set(i);
    }
    map<int, int> ordering = readOrdering();
    
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