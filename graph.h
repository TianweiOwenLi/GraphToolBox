#ifndef GRAPH_HEADER
#define GRAPH_HEADER

#include <vector>
#include <unordered_set>
#include <unordered_map>

using std::vector;
using std::pair;

template<typename T_vertex, typename T_edge> class Graph {

    std::unordered_map<int, std::unordered_set<int>> g;
    int graph_size;
    bool directed;
    
    bool verbose;

    // vertex and edge augmentation.
    // this design allows user to create a barebone graph first, 
    // and then add in things like weights, etc. sometime later. 
    std::unordered_map<int, pair<T_vertex, std::unordered_map<int, T_edge>>> augmentations;
    
    int reachability_dfs(vector<bool> &seen, int source);

    public: 
        Graph(int size, vector<pair<int, int>> edges, bool directed);
        bool reachable_from(int source);
        int connected_component_count();
        vector<pair<int, int>> dfs_num();
        vector<pair<int, int>> critical_edges();
        bool has_directed_cycle();
        bool has_critical_edge();
        bool get_verbose();
        void set_verbose(bool);
        int num_edges();
        bool is_tree();
        bool is_forest();
};

#endif