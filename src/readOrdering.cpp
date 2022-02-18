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