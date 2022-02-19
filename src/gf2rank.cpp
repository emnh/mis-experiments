/*
https://stackoverflow.com/questions/56856378/fast-computation-of-matrix-rank-over-gf2:

def gf2_rank(rows):
    """
    Find rank of a matrix over GF2.

    The rows of the matrix are given as nonnegative integers, thought
    of as bit-strings.

    This function modifies the input list. Use gf2_rank(rows.copy())
    instead of gf2_rank(rows) to avoid modifying rows.
    """
    rank = 0
    while rows:
        pivot_row = rows.pop()
        if pivot_row:
            rank += 1
            lsb = pivot_row & -pivot_row
            for index, row in enumerate(rows):
                if row & lsb:
                    rows[index] = row ^ pivot_row
    return rank
*/

unsigned long long gf2_rank(vector<hoodtype> rows) {
    unsigned long long ret = 0;
    while (rows.size() > 0) {
        hoodtype pivot_row = rows.back();
        rows.pop_back();
        if (pivot_row != hoodtype()) {
            ret += 1;
            hoodtype lsb;
            pivot_row.enumerate([&lsb](int i) {
                lsb.set(i);
                return true;
            });
            for (int i = 0; i < rows.size(); i++) {
                if ((rows[i] & lsb) != hoodtype()) {
                    rows[i] = rows[i] ^ pivot_row;
                }
            }
        }
    }
    return ret;
}