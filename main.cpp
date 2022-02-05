#include <iostream>
#include <vector>
#include <bitset>
#include <set>
#include <cmath>

using namespace std;

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

    const int operator< (const BitSet<N>& rhs) const noexcept {
        for (int i = 0; i < Nlong; i++) {
            if (data[i] ^ rhs.data[i]) {
                return data[i] < rhs.data[i];
            }
        }
        return 0;
    }
};

constexpr int N = 64;
typedef BitSet<N> hoodtype;

unsigned long long countUnions(vector<hoodtype>& neighbourhoods) {
    set<hoodtype> s1;
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

int main() {
    vector<hoodtype> neighbourhoods;
    for (int i = 0; i < 20; i++) {
        hoodtype hood;
        hood.set(i);
        // hood.set(i + 1);
        neighbourhoods.push_back(hood);
    }
    unsigned long long count = countUnions(neighbourhoods);
    cout << "count: " << count << endl;
    return 0;
}