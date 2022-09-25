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

/**
 * a Graph class.
 */
class Graph {

    unordered_map<int, unordered_set<int>> g;
    int graph_size;
    bool directed;

    static bool verbose;

    public:

        /**
         * Initializes a Graph instance.
         */
        Graph(int size, vector<pair<int, int>> edges, bool directed) {
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
         * performs a dfs to see if all components are reachable from a certain source.
         * 
         * @return a boolean value indicating if the entire graph is reachable from the given source.
         */
        bool reachable_from(int source) {
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

        // TODO generic dfs
        // TODO (another class) dijkstra
        // TODO is forest
        // TODO cache key computational results like dfs numbering and / or critical edges.


        /**
         * Performs iterative dfs and computes visit / finish timestamp of all vertices. 
         * 
         * @return a vector of pairs of integers, with the ith pair is the (visit, finish) time stamp for vertex i.
         */
        vector<pair<int, int>> dfs_num() {
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
        vector<pair<int, int>> critical_edges(){
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

            // identify critical edges after modified dfs numbering. 
            for (auto item : g) {
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
        bool has_directed_cycle() {
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
        bool has_critical_edge() {
            auto e = critical_edges();
            return !e.empty();
        }


        static bool get_verbose() {
            return verbose;
        }


        static void set_verbose(bool verbose_target_value) {
            verbose = verbose_target_value;
        }


        /**
         * Checks if an undirected graph is a tree. This is done by checking if the graph is connected and 
         * has n-1 (undirected) edges.
         * 
         * @return a boolean value indicating if the graph is indeed a tree.
         */
        bool is_tree() {
            assert(!directed);
            if (!reachable_from(0)) 
                return false;
            int counter = 0;
            for (auto item : g) 
                counter += item.second.size();
            counter /= 2; // since edges were double counted in previous loop
            return counter == graph_size - 1;
        }
};

bool Graph::verbose = false;


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
