#include "bfs.h"

#include <cstdlib>
#include <cstring>
#include <cstddef>
#include <cstdio>
#include <vector>
#include <omp.h>

#include "../common/CycleTimer.h"
#include "../common/graph.h"

#define ROOT_NODE_ID 0
#define NOT_VISITED_MARKER -1

void vertex_set_clear(vertex_set *list) {
    list->count = 0;
}

void vertex_set_init(vertex_set *list, int count) {
    list->max_vertices = count;
    list->vertices = (int *)malloc(sizeof(int) * list->max_vertices);
    vertex_set_clear(list);
}

/* Top-Down Approach */
void top_down_step(Graph g, vertex_set *frontier, vertex_set *new_frontier, int *distances) {
    std::vector<int> vec[32];
    #pragma omp parallel for
    for (int i = 0; i < frontier->count; i++) {
        const int node = frontier->vertices[i];
        const Vertex* out_begin = outgoing_begin(g, node);
        const Vertex* out_end = outgoing_end(g, node);

        for (const Vertex *outgoing = out_begin; outgoing < out_end; outgoing++) {
            if (distances[*outgoing] == NOT_VISITED_MARKER) {
                distances[*outgoing] = distances[node] + 1;
                vec[omp_get_thread_num()].push_back(*outgoing);
            }
        }
    }

    for (int k=0; k<32; k++)
        for (const int vtx : vec[k]) {
            int idx = new_frontier->count++;
            new_frontier->vertices[idx] = vtx;
        }
}

void bfs_top_down(Graph graph, solution *sol) {
    vertex_set list1, list2;
    vertex_set_init(&list1, graph->num_nodes);
    vertex_set_init(&list2, graph->num_nodes);

    vertex_set *frontier = &list1;
    vertex_set *new_frontier = &list2;

    // initialize all nodes to NOT_VISITED
    for (int i = 0; i < graph->num_nodes; i++)
        sol->distances[i] = NOT_VISITED_MARKER;

    // setup frontier with the root node
    frontier->vertices[frontier->count++] = ROOT_NODE_ID;
    sol->distances[ROOT_NODE_ID] = 0;

    while (frontier->count != 0) {
#ifdef VERBOSE
        double start_time = CycleTimer::currentSeconds();
#endif

        vertex_set_clear(new_frontier);
#pragma opm parallel for
        top_down_step(graph, frontier, new_frontier, sol->distances);

#ifdef VERBOSE
        double end_time = CycleTimer::currentSeconds();
        printf("frontier=%-10d %.4f sec\n", frontier->count, end_time - start_time);
#endif

        // swap pointers
        vertex_set *tmp = frontier;
        frontier = new_frontier;
        new_frontier = tmp;
    }
}

/* Buttom-Up Approach */
void bottom_up_step(Graph g, vertex_set *frontier, vertex_set *new_frontier, int *distances) {
    std::vector<int> vec[32];
    #pragma omp parallel for
    for (int i = 0; i < frontier->count; i++) {
        const int node = frontier->vertices[i];
        const Vertex* in_begin = incoming_begin(g, node);
        const Vertex* in_end = incoming_end(g, node);

        // attempt to add all neighbors to the new frontier
        for (const Vertex *incoming = in_begin; incoming < in_end; incoming++) {
            if (distances[*incoming] == NOT_VISITED_MARKER) {
                distances[*incoming] = distances[node] + 1;
                vec[omp_get_thread_num()].push_back(*incoming);
            }
        }
    }

    for (int k=0; k<32; k++)
        for (const int vtx : vec[k]) {
            int idx = new_frontier->count++;
            new_frontier->vertices[idx] = vtx;
        }
}

// Implements top-down BFS.
//
// Result of execution is that, for each node in the graph, the
// distance to the root is stored in sol.distances.
void bfs_bottom_up(Graph graph, solution *sol) {
    vertex_set list1, list2;
    vertex_set_init(&list1, graph->num_nodes);
    vertex_set_init(&list2, graph->num_nodes);

    vertex_set *frontier = &list1;
    vertex_set *new_frontier = &list2;

    // initialize all nodes to NOT_VISITED
    for (int i = 0; i < graph->num_nodes; i++)
        sol->distances[i] = NOT_VISITED_MARKER;

    // setup frontier with the root node
    frontier->vertices[frontier->count++] = ROOT_NODE_ID;
    sol->distances[ROOT_NODE_ID] = 0;

    while (frontier->count != 0) {
#ifdef VERBOSE
        double start_time = CycleTimer::currentSeconds();
#endif

        vertex_set_clear(new_frontier);
#pragma opm parallel for
        bottom_up_step(graph, frontier, new_frontier, sol->distances);

#ifdef VERBOSE
        double end_time = CycleTimer::currentSeconds();
        printf("frontier=%-10d %.4f sec\n", frontier->count, end_time - start_time);
#endif

        // swap pointers
        vertex_set *tmp = frontier;
        frontier = new_frontier;
        new_frontier = tmp;
    }
}

void bfs_hybrid(Graph graph, solution *sol) {
    // For PP students:
    //
    // You will need to implement the "hybrid" BFS here as
    // described in the handout.
}
