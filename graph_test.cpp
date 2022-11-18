
#include <vector>
#include <iostream>

#include "graph.cpp"

using std::vector;
using std::pair;
using std::make_pair;
using std::cout;
using std::endl;

typedef struct{} unit;

typedef struct VertexStruct {
    bool happy;
    VertexStruct(): happy(false) {}
} vertex_type;

typedef struct EdgeStruct {
    double flow;
    double cost;
    EdgeStruct(): flow(0.0), cost(4.0) {}
} edge_type;


bool vec_has_undir_edge(vector<pair<int, int>>& vec, pair<int, int> edge) {
    for (auto e : vec) {
        if (e.first == edge.second && e.second == edge.first)
            return true;
        if (e.first == edge.first && e.second == edge.second)
            return true;
    }

    return false;
}


int main() {

    cout << "\n[GraphToolBox] Performing unit tests: \n" << endl;

    cout << "Testing graph reachability" << endl;
    Graph<unit, unit> G(6, {{0,1}, {1,2}, {3,1}, {3,4}, {5,2}, {2,3}}, false);
    assert(G.reachable_from(0));

    cout << "Testing DFS numbering" << endl;
    auto timestamps = G.dfs_num();
    vector<pair<int, int>> timestamps_refsol \
            {{0,11},{1,10},{3,6},{2,9},{7,8},{4,5}};
    assert(timestamps == timestamps_refsol);

    cout << "Testing critical edge detection";
    auto ce = G.critical_edges();
    assert(ce.size() == 3);
    assert(vec_has_undir_edge(ce, {0,1}));
    assert(vec_has_undir_edge(ce, {2,5}));
    assert(vec_has_undir_edge(ce, {4,3}));


    cout << "\nTesting trees, forests and connected componentes" << endl;
    
    assert(!G.is_tree());

    Graph<unit, unit> H(6, {{1,2}, {0,1}, {1,3}, {4,2}, {5,3}}, false);
    assert(H.is_tree());
    assert(H.is_forest());

    Graph<unit, unit> I1(10, {{0,1}, {3,4}, {6,7}, {7,5}, {7,8}, {8,6}}, false);
    Graph<unit, unit> I2(10, {{0,1}, {3,4}, {9,7}, {7,5}, {7,8}, {8,6}}, false);
    assert(I1.connected_component_count() == 5);
    assert(!I1.is_forest());
    assert(I2.is_forest());


    cout << "Testing vertex and edge data augmentations" << endl;

    Graph<vertex_type, edge_type> J(3, {{0,1}, {2,0}}, true);
    J.ptr_to_vertex_data(0)->happy = true;
    J.ptr_to_edge_data(2,0)->flow = 3.5;
    assert(J.ptr_to_vertex_data(0)->happy);
    assert(!J.ptr_to_vertex_data(1)->happy);
    assert(!J.ptr_to_vertex_data(2)->happy);
    auto vale1 = J.ptr_to_edge_data(2,0)->flow;
    auto vale2 = J.ptr_to_edge_data(0,1)->cost;
    assert(vale1 > 3.49 && vale1 < 3.51);
    assert(vale2 > 3.9 && vale2 < 4.1);

    cout << "\nAll Tests Passed! \n\n";


    return 0;

}