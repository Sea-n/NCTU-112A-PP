#include "page_rank.h"

#include <stdlib.h>
#include <utility>
#include <vector>
#include <cmath>
#include <omp.h>

#include "../common/CycleTimer.h"
#include "../common/graph.h"

using namespace std;

// pageRank --
//
// g:           graph to process (see common/graph.h)
// solution:    array of per-vertex vertex scores (length of array is num_nodes(g))
// damping:     page-rank algorithm's damping parameter
// convergence: page-rank algorithm's convergence threshold
//
void pageRank(Graph g, double *solution, double damping, double convergence) {
    // initialize vertex weights to uniform probability.
    // Double precision scores are used to avoid underflow for large graphs
    int numNodes = num_nodes(g);
    double equal_prob = 1.0 / numNodes;
    double rem_prob = (1.0 - damping) / numNodes;

    /*
     * For PP students: Implement the page rank algorithm here.
     * You are expected to parallelize the algorithm using OpenMP.
     * Your solution may need to allocate (and free) temporary arrays.
     *
     * Basic page rank pseudocode is provided below to get you started:
     */

    // initialization: see example code above
    // score_old[vi] = 1/numNodes;
    vector<double> score_old(numNodes);
    vector<double> score_new(numNodes, equal_prob);

    bool converged = false;
    while (!converged) {
        // Copy score_{new => old} and reset score_new later
        score_old = score_new;

        // Calc for dangling nodes (those with no outgoing edges)
        double score_dangling = 0.0;
        #pragma omp parallel for reduction(+:score_dangling)
        for (int vj = 0; vj < numNodes; vj++) {
            if (outgoing_size(g, vj) == 0)
                score_dangling += score_old[vj];
        }
        // Reset score_new with dangling score
        fill(score_new.begin(), score_new.end(), score_dangling / numNodes);

        // Calculate score for each node
        #pragma omp parallel for
        for (int vi = 0; vi < numNodes; vi++) {
            const Vertex* in_end = incoming_end(g, vi);
            for (const Vertex* vj = incoming_begin(g, vi); vj != in_end; vj++) {
                score_new[vi] += score_old[*vj] / outgoing_size(g, *vj);
            }

            // Apply damping factor
            score_new[vi] = (damping * score_new[vi]) + rem_prob;
        }

        // Check for convergence
        double global_diff = 0.0;
        #pragma omp parallel for reduction(+:global_diff)
        for (int vi = 0; vi < numNodes; vi++) {
            global_diff += abs(score_new[vi] - score_old[vi]);
        }
        converged = (global_diff < convergence);
    }

    // Copy the final scores to the solution array
    copy(score_new.begin(), score_new.end(), solution);
}
