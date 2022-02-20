#include <iostream>
#include <cstdlib>
#include <ctime>
#include <sstream>

// Adapted from https://www.tutorialspoint.com/cplusplus-program-to-implement-treap.

struct TreapNode {
public:
    int index;
    int key;
    int priority;
    int left;
    int right;

    TreapNode() {
        this->index = -1;
        this->key = -1;
        this->priority = -1;
        this->left = -1;
        this->right = -1;
    }

    TreapNode(int index, int key, int priority) {
        this->index = index;
        this->key = key;
        this->priority = priority;
        this->left = -1;
        this->right = -1;
    }
};

class Treap {
public:
    TreapNode nodes[N];
    int rootIndex = -1;
    unsigned int firstFree = 0;
    unsigned int count = 0;

    Treap() {
        for (int i = 0; i < N; i++) {
            nodes[i] = TreapNode(i, -1, -1);
        }
    }

    void rotLeft(int& index) {
        // cerr << "rotLeft: " << index << endl;
        TreapNode& root = nodes[index];
        TreapNode& R = nodes[root.right];
        const int Xindex = nodes[root.right].left;
        R.left = root.index;
        root.right = Xindex;
        // swap(nodes[index], R);
        // swap(nodes[index].index, R.index);
        index = R.index;
        // nodes[index].index = R.index;
    }

    void rotRight(int& index) {
        // cerr << "rotRight: " << index << endl;
        TreapNode& root = nodes[index];
        TreapNode& L = nodes[root.left];
        const int Yindex = nodes[root.left].right;
        L.right = root.index;
        root.left = Yindex;
        // swap(nodes[index], L);
        // swap(nodes[index].index, L.index);
        index = L.index;
    }

    void insertNode(int key, int priority) {
        insertNode(rootIndex, key, priority);
    }

    void insertNode(int& index, int key, int priority) {
        if (index == -1) {
            index = firstFree;
            nodes[index] = TreapNode(index, key, priority);
            firstFree++;
            count++;
            assert (firstFree < N && count < N);
            return;
        }
        TreapNode& root = nodes[index];
        if (key < root.key) {
            insertNode(root.left, key, priority);
            if (root.left != -1 && nodes[root.left].priority > root.priority) {
                rotRight(index);
            }
        } else {
            insertNode(root.right, key, priority);
            if (root.right != -1 && nodes[root.right].priority > root.priority) {
                rotLeft(index);
            }
        }
    }

    string display() const {
        return display(rootIndex);
    }

    string display(int index) const {
        if (index == -1) {
            return "";
        }
        const TreapNode& root = nodes[index];
        stringstream ss;
        string s1 = display(root.left);
        string s2 = display(root.right);
        ss << root.key << "|" << root.priority << ":";
        if (s1 != "" && s2 != "") {
            ss << "(" << s1 << ", " << s2 << ")";
        } else if (s1 != "") {
            ss << "(" << s1 << ")";
        } else if (s2 != "") {
            ss << "(" << s2 << ")";
        } else {
            ss << "(" << root.key << ")";
        }
        return ss.str();
    }

    vector<int> getChildren(int index) {
        vector<int> children;
        if (index == -1) {
            return children;
        }
        children.push_back(index);
        unsigned int i = 0;
        while (i < children.size()) {
            const TreapNode& child = nodes[children[i]];
            if (child.left != -1) {
                children.push_back(child.left);
            }
            if (child.right != -1) {
                children.push_back(child.right);
            }
            i++;
        }
        return children;    
    }

    vector<vector<int>> getAllBags() {
        vector<vector<int>> bags;
        for (unsigned int k = 0; k < count; k++) {
            const vector<int> children = getChildren(k);
            if (children.size() > 0) {
                bags.push_back(children);
            }
        }
        return bags;
    }
};

Treap toTreap(vector<int> ordering) {
    Treap treap;
    for (unsigned int i = 0; i < ordering.size(); i++) {
        treap.insertNode(ordering[i], i);
        // cerr << "inserted " << ordering[i] << " at " << i << endl;
        // for (int j = 0; j < i + 1; j++) {
            // cerr << "j: " << j << ": index=" << treap.nodes[j].index << " key=" << treap.nodes[j].key << " pri=" << treap.nodes[j].priority;
            // cerr << " left=" << treap.nodes[j].left << " right=" << treap.nodes[j].right << endl;
        // }
    }
    vector<vector<int>> bags = treap.getAllBags();
    for (unsigned int i = 0; i < bags.size(); i++) {
        vector<int> children = bags[i];
        cerr << "children: ";
        for (unsigned int k = 0; k < children.size(); k++) {
            cerr << children[k] << " ";
        }
        cerr << endl;
    }
    
    return treap;
}