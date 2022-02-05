#include <iostream>
#include <vector>
#include <bitset>
#include <set>
#include <unordered_set>
#include <cmath>
#include <chrono>
#include <functional>
#include <deque>

using namespace std;
using time_interval_t = std::chrono::microseconds;
using myClock = std::chrono::high_resolution_clock;

const bool DEBUG = true;

template<std::size_t N>
class BitSet {
    static constexpr int Nlong = ceil(N / 64);
    uint64_t data[Nlong];
public:
    BitSet() {
        for (int i = 0; i < Nlong; i++) {
            data[i] = 0;
        }
    }

    void set(int i) {
        data[i / 64] |= (1 << (i % 64));
    }

    void reset(int i) {
        data[i / 64] &= ~(1 << (i % 64));
    }

    const bool test(int i) const {
        return data[i / 64] & (1 << (i % 64));
    }

    void flip(int i) {
        data[i / 64] ^= (1 << (i % 64));
    }

    const int count() const {
        int cnt = 0;
        for (int i = 0; i < Nlong; i++) {
            cnt += __builtin_popcountll(data[i]);
        }
        return cnt;
    }

    void print(std::ostream &os) const {
        for (int i = 0; i < Nlong; i++) {
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
                if (f(j + i * 64)) {
                    return true;
                }
                j++;
                tmpX >>= (setbit + 1);
            }
        }
        return false;
    }
};

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
    
    cerr << "HOODS: " << endl;
    for (hoodtype h : s1) {
        h.print(cerr);
        cerr << endl;
    }

    return s1.size();
}

unsigned long long countUnions2(vector<hoodtype>& neighbourhoods) {
    unordered_set<hoodtype, node_hash> s1(1 << neighbourhoods.size());
    vector<vector<hoodtype>> vs = *(new vector<vector<hoodtype>>());
    for (int i = 0; i <= neighbourhoods.size(); i++) {
        vs.push_back(vector<hoodtype>());
    }
    s1.insert(hoodtype());
    vs[0].push_back(hoodtype());
    for (int i = 0; i < neighbourhoods.size(); i++) {
        hoodtype current = neighbourhoods[i];
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

vector<hoodtype> connectedComponents(vector<hoodtype>& neighbourhoods, hoodtype subset) {
    hoodtype seen;
    vector<hoodtype> ret;

    subset.enumerate([&neighbourhoods, &subset, &seen, &ret] (int i) {
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
            
            (neighbourhoods[top] & subset).enumerate([&q, &seen] (int j) {
                if (!seen.test(j)) {
                    q.push_back(j);
                    seen.set(j);
                }
                return false;
            });
        }
        if (current != hoodtype()) {
            ret.push_back(current);
        }

        return false;
    });

    if (ret.size() > 1) {
        cerr << "components: " << ret.size() << endl;
    }
    
    return ret;
}

unsigned long long CCMIS(int depth, vector<hoodtype>& neighbourhoods, const hoodtype mask, const hoodtype P, const hoodtype X) {
    for (int i = 0; i < depth; i++) {
        cerr << "--";
    }
    cerr << "P: ";
    P.print(cerr);
    cerr << " X: ";
    X.print(cerr);
    cerr << endl;

    if ((P | X) == hoodtype()) {
        cerr << "return 1" << endl;
        return 1;
    }
    // if (P == hoodtype() && X != hoodtype()) {
    //     cerr << "return 0" << endl;
    //     return 0;
    // }
    bool found = X.enumerate([&neighbourhoods, &mask, &P] (int i) {
        const hoodtype Xhood = neighbourhoods[i] & mask;
        // if ((Xhood - P) == Xhood) {
        if ((Xhood & P) == hoodtype()) {
            cerr << "w in X: " << i << " hood: ";
            Xhood.print(cerr);
            cerr << " P: ";
            P.print(cerr);
            cerr << endl;
            return true;
        }
        return false;
    });
    if (found) {
        cerr << "return 0" << endl;
        return 0;
    }

    unsigned long long count = 0;

    vector<hoodtype> components = connectedComponents(neighbourhoods, (P | X) & mask);
    if (components.size() > 1) {

        count = 1;
        for (hoodtype component : components) {
            const hoodtype newmask = mask & component;
            count *= CCMIS(depth + 1, neighbourhoods, newmask, P & newmask, X & newmask);
        }

        return count;
    }

    int v = -1;
    P.enumerate([&v] (int i) {
        v = i;
        return true;
    });
    cerr << "V: " << v << " VHOOD: ";
    (neighbourhoods[v] & mask).print(cerr);
    cerr << endl;
    hoodtype PminNv = P - (neighbourhoods[v] & mask);
    PminNv.reset(v);
    hoodtype XminNv = X - (neighbourhoods[v] & mask);
    count = CCMIS(depth + 1, neighbourhoods, mask, PminNv, XminNv);
    hoodtype Pminv = P;
    Pminv.reset(v);
    hoodtype Xuv = X;
    Xuv.set(v);
    count += CCMIS(depth + 1, neighbourhoods, mask, Pminv, Xuv);

    return count;
}

int main() {
    vector<hoodtype> neighbourhoods;
    const int maxi = 4;
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
    cerr << "NEIGHBOURHOODS2: " << endl;
    for (hoodtype h : neighbourhoods2) {
        h.print(cerr);
        cerr << endl;
    }
    
    auto start3 = myClock::now();
    unsigned long long count3 = CCMIS(0, neighbourhoods2, P, P, X);
    const auto elapsed3 = std::chrono::duration_cast<time_interval_t>(myClock::now() - start3);
    cout << "count3: " << count3 << " in " << elapsed3.count() / 1000 << endl;

    return 0;
}