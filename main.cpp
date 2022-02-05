#include <iostream>
#include <vector>
#include <bitset>
#include <set>
#include <unordered_set>
#include <cmath>
#include <chrono>

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

    bool test(int i) {
        return data[i / 64] & (1 << (i % 64));
    }

    void flip(int i) {
        data[i / 64] ^= (1 << (i % 64));
    }

    int count() {
        int cnt = 0;
        for (int i = 0; i < Nlong; i++) {
            cnt += __builtin_popcountll(data[i]);
        }
        return cnt;
    }

    void print(std::ostream &os) {
        for (int i = 0; i < Nlong; i++) {
            os << bitset<64>(data[i]);
        }
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
        uint64_t ret;
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
    return s1.size();
}

unsigned long long countUnions2(vector<hoodtype>& neighbourhoods) {
    unordered_set<hoodtype, node_hash> s1(1 << neighbourhoods.size());
    vector<hoodtype> vs[neighbourhoods.size() + 1];
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
    return s1.size();
}

int main() {
    vector<hoodtype> neighbourhoods;
    for (int i = 0; i < 20; i++) {
        hoodtype hood;
        hood.set(i);
        // hood.set(i + 1);
        neighbourhoods.push_back(hood);
    }
    auto start = myClock::now();
    unsigned long long count = countUnions(neighbourhoods);
    const auto elapsed = std::chrono::duration_cast<time_interval_t>(myClock::now() - start);
    cout << "count1: " << count << " in " << elapsed.count() / 1000 << endl;
    
    auto start2 = myClock::now();
    unsigned long long count2 = countUnions2(neighbourhoods);
    const auto elapsed2 = std::chrono::duration_cast<time_interval_t>(myClock::now() - start2);
    cout << "count2: " << count2 << " in " << elapsed2.count() / 1000 << endl;
    return 0;
}