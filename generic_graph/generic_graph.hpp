#if !defined(GENGRAPH)
#define GENGRAPH

#include <unordered_map>
#include <vector>

using std::vector;
using std::pair;
using std::unordered_map;

template <typename Tv, typename Te> class GenGraph {
    unordered_map<Tv, unordered_map<Tv, Te>> g;

    public:
        GenGraph(vector<Tv> vertices, vector<std::tuple<Tv, Tv, Te>> edges) {
            for(auto v : vertices) {
                g[v] = NULL;
            }

            for (std::tuple<Tv,Tv,Te> e : edges) {
                Tv u = std::get<0>(e), v = std::get<1>(e);
                
                assert(u != v); // self-edges are not allowed
                assert(has(u) && has(v));
                g[u][v] = Te();
                
            }
        }

        inline bool has(Tv vertex) {
            return g.find(v) != g.end();
        }
};

#endif // GENGRAPH


