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