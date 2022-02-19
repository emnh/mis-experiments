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
            if (a >= N || b >= N) {
                throw out_of_range("graph size: N");
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

    bool writeGraphText = false;

    if (writeGraphText) {
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
    }

    // int i = system("ndmetis -ccorder graph.txt");
    // cerr << "SKIPPING METIS" << endl;
    // cerr << realEdges << endl;
    cerr << "graph vertices edges: " << neighbourhoods.size() << " " << edges / 2 << endl; 

    return neighbourhoods;
}
