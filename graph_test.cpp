
#include <vector>
#include <iostream>

#include "graph.h"

using std::vector;
using std::pair;
using std::make_pair;
using std::cout;
using std::endl;

// TODO revise tester, increase comment / modularity.

int main() {
    vector<pair<int, int>> v;
    v.push_back(make_pair(0,1));
    v.push_back(make_pair(1,2));
    v.push_back(make_pair(3,1));
    v.push_back(make_pair(3,4));
    v.push_back(make_pair(5,2));
    v.push_back(make_pair(2,3));
    Graph<void*, void*> G(6, v, false);
    G.set_verbose(true);

    cout << "G is reachable from 0? " << std::boolalpha << G.reachable_from(0) << endl;
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
    Graph<void*, void*> H(6, w, false);
    cout << "is H a tree? " << H.is_tree() << endl;

    vector<pair<int, int>> x;
    x.push_back(make_pair(0,1));
    x.push_back(make_pair(3,4));
    x.push_back(make_pair(6,7));
    x.push_back(make_pair(7,5));
    x.push_back(make_pair(7,8));
    x.push_back(make_pair(8,6));
    Graph<void*, void*> I(10, x, false);
    cout << "number of connected components in I is " << I.connected_component_count() << endl;
    cout << "Is I a forest? " << I.is_forest() << endl;

    return 0;

}