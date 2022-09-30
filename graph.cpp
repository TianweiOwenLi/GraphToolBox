#include <iostream>

#include <assert.h>

#include <stack>
#include <queue>

#include <set>
#include <unordered_set>

#include <list>
#include <vector>

#include <random>
#include <algorithm>
#include <cmath>
#include <string>

#include <unordered_map>

#include <functional>

#include "graph_utility.h"
#include "graph.h"

#define dbg(x) std::cout<<#x<<" = "<<x<<'\n'

using std::cout;
using std::endl;

using std::vector;
using std::stack;
using std::queue;

using std::pair;
using std::make_pair;

using std::unordered_set;
using std::unordered_map;

using std::function;

using std::min;
using std::max;


// TODO generic dfs AND reduce code reuse.
// TODO (another class) dijkstra
// TODO is forest
// TODO cache key computational results like dfs numbering and / or critical edges.
// TODO Prufer Code
// TODO lambda & management


/**
 * A class for unweighted enumerable graphs.
 */
Graph::Graph(int size, vector<pair<int, int>> edges, bool directed) {
    assert(size > 0);
    for (auto e : edges) {
        // identify self-edges, as well as vertices with bad values.
        assert(e.first != e.second); 
        assert(e.first >= 0 && e.second >= 0 && e.first < size && e.second < size);

        g[e.first].insert(e.second);
        if (!directed)
            g[e.second].insert(e.first);
    }
    graph_size = size;
    this->directed = directed;
}


/**
 * Performs a dfs to see if all components are reachable from a certain source.
 * 
 * @return a boolean value indicating if the entire graph is reachable from the given source.
 */
bool Graph::reachable_from(int source) {
    assert(source >= 0 && source < graph_size);

    stack<int> dfs_stack;
    vector<bool> seen(graph_size);

    seen[0] = true;
    dfs_stack.push(source);

    while (!dfs_stack.empty()) {
        int v = dfs_stack.top();
        dfs_stack.pop();
        for (int neighbor : g[v]) {
            if (!seen[neighbor]) {
                seen[neighbor] = true;
                dfs_stack.push(neighbor);
            }
        }
    }

    return std::accumulate(seen.begin(), seen.end(), 0) == graph_size; // counts number of true in seen.
}

/**
 * Counts the number of connected components in an undirected graph.
 * 
 * @return an integer indicating the number of connected components in a graph.
 */
int Graph::connected_component_count() {
    assert(!directed);

    stack<int> dfs_stack;
    vector<bool> seen(graph_size);

    // starts with the given vertex, and mark everything reachable as "seen".
    // returns 
    function<int(int)> mark_reached_vertices_from = [&](int source) -> int {
        assert(dfs_stack.empty());
        if (seen[source]) return 0;
        seen[source] = true;
        dfs_stack.push(source);

        while (!dfs_stack.empty()) {
            int v = dfs_stack.top();
            dfs_stack.pop();
            for (int neighbor : g[v]) {
                if (!seen[neighbor]) {
                    seen[neighbor] = true;
                    dfs_stack.push(neighbor);
                }
            }
        }

        return 1;
    };

    int component_counter{ 0 };

    for (int i = 0; i < graph_size; i++) component_counter += mark_reached_vertices_from(i);

    return component_counter;

}


/**
 * Performs iterative dfs and computes visit / finish timestamp of all vertices. 
 * 
 * @return a vector of pairs of integers, with the ith pair is the (visit, finish) time stamp for vertex i.
 */
vector<pair<int, int>> Graph::dfs_num() {
    vector<bool> seen(graph_size);
    int current_time = 0;
    vector<pair<int, int>> timestamps(graph_size);
    
    function<void(int)> dfs = [&](int u) -> void {
        if (!seen[u]) { 
            seen[u] = true;
            timestamps[u].first = current_time++; // visit

            for (int neighbor : g[u]) dfs(neighbor); // rec

            // TODO verbose
            
            timestamps[u].second = current_time++; // finish
        } // revisit: trivial
    };

    for (int i = 0; i < graph_size; i++) dfs(i);  // iterative dfs
    return timestamps;
}


/**
 * Finds all critical edge of a graph. (an edge is called "critical" if removing it will increment the 
 * number of connected components by 1). Only works for undirected graphs. 
 * 
 * TODO annotate intuition of this algorithm since it is hard.
 * 
 * @return a vector of all identified critical edges.
 */
vector<pair<int, int>> Graph::critical_edges(){
    assert(! this->directed);
    vector<pair<int, int>> num = dfs_num();

    vector<bool> seen(graph_size);
    int current_time = -1;
    vector<pair<int, int>> timestamps(graph_size);
    stack<int> path_stack;
    path_stack.push(-1); // placeholder "parent of all"
    
    function<int(int)> dfs = [&](int u) -> int {
        if (seen[u]) { // revisit
            return timestamps[u].first;
        } else {
            seen[u] = true;
            int parent = path_stack.top();
            path_stack.push(u);
            int min_ancestor_time = ++current_time;
            timestamps[u].first = current_time; // visit

            for (int neighbor : g[u]) {
                if (neighbor == parent)
                    continue;
                int val = dfs(neighbor); // rec
                min_ancestor_time = min(min_ancestor_time, val);
            }

            path_stack.pop();
            
            timestamps[u].second = min_ancestor_time; // finish

            if (verbose) // TODO modify verbose
                cout << u << ": (" << timestamps[u].first << ", " <<timestamps[u].second << ")\n";

            return timestamps[u].second;
        }
    };

    for (int i = 0; i < graph_size; i++) dfs(i);  // iterative dfs

    vector<pair<int, int>> critical_edges;

    for (auto item : g) { // identify critical edges after performing modified dfs numbering. 
        int u = item.first;
        for (auto v : item.second) {
            if (u > v) 
                continue; // avoids duplicate edges
            int u1 = timestamps[u].first; int u2 = timestamps[u].second;
            int v1 = timestamps[v].first; int v2 = timestamps[v].second;
            if (! (gutil::gge_triplet(u1, v1, u2) || gutil::gge_triplet(v1, u1, v2)) )
                critical_edges.push_back(make_pair(u, v));
        }
    }

    return critical_edges;
}


/**
 * Checks if the graph has directed cycles. Directed cycles are a collection of directed edges that connect 
 * head to tail and form a loop, ie. (v1, v2), (v2, v3), (v3, v1). Only works for directed graphs.
 * 
 * @return a boolean value indicating the presence of any cycle.
 */
bool Graph::has_directed_cycle() {
    assert(directed);
    auto timestamps = dfs_num();
    for (auto item : g) {
        int u = item.first;
        for (int v : item.second) {
            // checks the dfs numbering of such an edge. If a back edge exists, a directed cycle exists.
            if (timestamps[u].first >= timestamps[v].first && timestamps[u].second <= timestamps[v].second) 
                return true;
        }
    }
    return false;
}


/**
 * Checks if a graph has any critical edge. Only works for undirected graphs. 
 * 
 * @return a boolean value indicating the presence of any critical edge. 
 */
bool Graph::has_critical_edge() {
    auto e = critical_edges();
    return !e.empty();
}


bool Graph::get_verbose() {
    return verbose;
}


void Graph::set_verbose(bool verbose_target_value) {
    verbose = verbose_target_value;
}

/**
 * Counts the number of edges in a graph. Works for both directed and undirected graphs.
 */
int Graph::num_edges() {
    int counter = 0;
    for (auto item : g)
        counter += item.second.size();
    if (!directed)
        counter /= 2;
    return counter;
}


/**
 * Checks if an undirected graph is a tree. This is done by checking if the graph is connected and 
 * has n-1 (undirected) edges.
 * 
 * @return a boolean value indicating if the graph is indeed a tree.
 */
bool Graph::is_tree() {
    assert(!directed);
    if (!reachable_from(0)) 
        return false;
    return num_edges() + 1 == graph_size;
}

/**
 * Checks if an undirected graph is a forest. A forest is an undirected graph where each connected 
 * component is a tree.
 * 
 * @return a boolean value indicating if the graph is indeed a forest.
 */
bool Graph::is_forest() {
    assert(!directed);
    return num_edges() + connected_component_count() == graph_size;  //place holder
}


bool Graph::verbose = false; // TODO implement verbose mode
