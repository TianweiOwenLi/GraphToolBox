
#include <vector>
#include <iostream>

#include "graph.h"

using std::vector;
using std::pair;
using std::make_pair;
using std::cout;
using std::endl;

int main() {
    vector<pair<int, int>> v;
    v.push_back(make_pair(0,1));
    v.push_back(make_pair(1,2));
    v.push_back(make_pair(3,1));
    v.push_back(make_pair(3,4));
    v.push_back(make_pair(5,2));
    v.push_back(make_pair(2,3));
    Graph G(6, v, false);
    Graph::set_verbose(true);

    cout << std::boolalpha << G.reachable_from(0) << endl;
    auto timestamps = G.dfs_num();
    auto vertex = 0;
    cout << "dfs numbering: \n";
    for (auto item : timestamps) {
        cout << vertex << ": (" << item.first << ", " << item.second << ")\n";
        vertex++;
    }

    cout << "\ncritical edge detection: \n";
    // cout << std::boolalpha << G.has_critical_edge() << endl;
    auto ce = G.critical_edges();
    cout << "\nfound ones:\n";
    for (auto item : ce) {
        cout << "(" << item.first << ", " << item.second << ")\n";
    }

    cout << "\nis G a tree? " << G.is_tree() << endl;
    

    vector<pair<int, int>> w;
    w.push_back(make_pair(1,2));
    w.push_back(make_pair(0,1));
    w.push_back(make_pair(1,3));
    w.push_back(make_pair(4,2));
    w.push_back(make_pair(5,3));
    Graph H(6, w, false);
    cout << "is H a tree? " << H.is_tree() << endl;

    return 0;

}