#include<bits/stdc++.h>
//Change here to apply or not apply PDR
//#include "GraphBuilder.cpp"
#include "GraphBuilder_Optimized_PDR.cpp"
#include <chrono>
#include "platform_atomics.h"
#include "pvector.h"

using namespace std;

const Weight kDistInf = numeric_limits<Weight>::max()/2;
const Node kMaxBin = numeric_limits<Node>::max()/2;
const Node kBinSizeThreshold = 100000;


void RelaxEdges(const WCSRGraph& g, Node u, Weight delta,
                pvector<Weight> &dist, vector <vector<Node>> &local_bins) 
{   
    for (int i = g.row_ptr[u]; i < g.row_ptr[u + 1]; ++i) {    
        int v = g.col_indices[i];
        Weight w = g.weights[i];

        //cout<<"v = "<<v<<"  w= "<<w<<endl;

        Weight old_dist = dist[v];
        Weight new_dist = dist[u] + w;

        // Update the distance
        // Is it feasible to use the local distances
        
        while (new_dist < old_dist) {
            
            if (compare_and_swap(dist[v], old_dist, new_dist))
            {
                Node dest_bin = floor(new_dist/delta);
                if (dest_bin >= local_bins.size())
                    local_bins.resize(dest_bin+1);
                    
                local_bins[dest_bin].push_back(v);
                break;

            }
            old_dist = dist[v]; // swap failed, recheck dist update & retry
        }
    }
}

Weight calculate_new_delta(Weight min_delta, Weight current_delta, float f_cd) {
    Weight new_delta = max(min_delta, (Weight)ceil(f_cd/1.20)); // Placeholder for adaptive logic
    return new_delta;
}



pvector<Weight> delta_stepping(int p, const WCSRGraph& graph, int source, Weight delta) {

    Weight min_delta = delta;
    delta = (Weight) min((Weight)ceil(1000*.5), (Weight)min_delta*3);
    auto start = std::chrono::high_resolution_clock::now();   
    int num_vertices = graph.getNumVertices();
    int num_edges = graph.getNumEdges();
    pvector<Weight> dist(num_vertices, kDistInf);
    dist[source] = 0;

    //Initialize a vector for frontier list
    pvector<Node> frontier(num_edges);
    omp_set_num_threads(p);

    //printf("Vertics = %d Edges = %d\n",num_vertices, num_edges);
    //cout<<frontier.size()<<endl;

    /**shared_indexes: This array is used to keep track of the indices of the current and next bins during the algorithm's execution. 
    It's a two-element array because of double buffering.
    The index curr refers to the current bin index, while the index next refers to the next bin index. 
    The use of bitwise AND operation (&1) ensures that the indices toggle between 0 and 1, effectively enabling double buffering.

    frontier_tails: Similarly, this array tracks the sizes of the frontiers for the current and next iterations. 
    frontier_tails[curr] holds the size of the current frontier, 
    and frontier_tails[next] holds the size of the frontier for the next iteration.

    Initialization:

    shared_indexes[0] = 0: Initializes the current bin index to 0.
    shared_indexes[1] = kMaxBin: Initializes the next bin index to a value (kMaxBin) that indicates termination.
    frontier_tails[0] = 1: Initializes the size of the current frontier to 1 (starting with the source node).
    frontier_tails[1] = 0: Initializes the size of the next frontier to 0 (no elements initially).
    **/

   Node shared_indexes[2] = {0, kMaxBin};
   Node frontier_tails[2] = {1, 0};


   frontier[0]= source;
   
   #pragma omp parallel
   {
        auto TStart = std::chrono::high_resolution_clock::now();
        auto TEnd = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(TEnd - TStart);

        //printf("Thread number = %d \n", omp_get_thread_num());
        
        Node iter = 0;
        vector<vector<Node>> local_bins(0);
        Weight new_delta=min_delta;
        Weight curr_delta = min_delta;
        float f_cd = min_delta;
        //cout<<"Size of the bins"<<local_bins.size()<<endl;

        // Run until current bin becomes infinte
        while (shared_indexes[iter&1] != kMaxBin) {

            TStart = std::chrono::high_resolution_clock::now();
            TEnd = std::chrono::high_resolution_clock::now();
            
            //Toggle the values
            Node &curr_bin_index = shared_indexes[iter&1];
            Node &next_bin_index = shared_indexes[(iter+1)&1];
            Node &curr_frontier_tail = frontier_tails[iter&1];
            Node &next_frontier_tail = frontier_tails[(iter+1)&1];

            //cout<<"Current bin index: "<<curr_bin_index<<"  Next bin idx: "<<next_bin_index<<" Curr Front tail: "<<curr_frontier_tail<<" Next front tail: "<<next_frontier_tail<<endl;
             // **Adaptive delta update:**
            if(f_cd>(float)min_delta)
                f_cd = f_cd/1.20;
            
            new_delta = calculate_new_delta(min_delta, curr_delta, f_cd);  // New adaptive delta

            // Initial updates
            #pragma omp for nowait schedule(dynamic, 64)
            for (Node i=0; i < curr_frontier_tail; i++) {
                Node u = frontier[i];
                if (dist[u] >= curr_delta * static_cast<Weight>(curr_bin_index))
                
                RelaxEdges(graph, u, new_delta, dist, local_bins);
            }

            //Secondary updates
            while (curr_bin_index < local_bins.size() &&
                !local_bins[curr_bin_index].empty() &&
                local_bins[curr_bin_index].size() < kBinSizeThreshold)
            {
                vector<Node> curr_bin_copy = local_bins[curr_bin_index];
                //clearing the local vector so that new updates can be identified
                local_bins[curr_bin_index].resize(0);

                for(Node u: curr_bin_copy)
                    RelaxEdges(graph, u, new_delta,dist, local_bins);

            }



            // Update the bin index to select next iteration frontier
            for (Node i=curr_bin_index; i < local_bins.size(); i++) 
            {
                if (!local_bins[i].empty()) 
                {
                    #pragma omp critical
                    next_bin_index = min(next_bin_index, i);
                    break;
                }
            }

            TEnd= std::chrono::high_resolution_clock::now();
            duration = duration + std::chrono::duration_cast<std::chrono::milliseconds>(TEnd - TStart);
            //Need to synchronize here. Each thread will update the bins and we need to consider all of them
            #pragma omp barrier



            //Update the current bin to Max and size 0 as it will be toggled
            #pragma omp single nowait
            {
                curr_bin_index = kMaxBin;
                curr_frontier_tail = 0;
                
            }

            //Update the next bin from all local bins and add to the frontier
            if (next_bin_index < local_bins.size()) {
                Node copy_start = fetch_and_add(next_frontier_tail,
                                                local_bins[next_bin_index].size());
                copy(local_bins[next_bin_index].begin(),
                    local_bins[next_bin_index].end(), frontier.data() + copy_start);
                local_bins[next_bin_index].resize(0);
            }
            curr_delta = new_delta;
            #pragma omp barrier
            iter++;

        }

        /**
        #pragma omp critical
        {

           
            cout<<"Thread: "<< omp_get_thread_num()<<" Time: "<<duration.count()<<" Iteration: "<<iter<<endl;
        }
        **/
   }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout<<duration.count()<<endl;
    return dist;
}




std:: vector<Weight> dijkstra(const WCSRGraph& graph, int source) {
    auto start = std::chrono::high_resolution_clock::now();
    int num_vertices = graph.getNumVertices();
    
    std:: vector<Weight> dist(num_vertices, kDistInf);
    dist[source] = 0;
    //for (int i=0; i<dist.size(); i++)
        //cout<<dist[i]<<" ";
    std::priority_queue<std::pair<Weight, int>, std::vector<std::pair<Weight, int>>, std::greater<std::pair<Weight, int>>> pq;
    pq.push({0, source});

    while (!pq.empty()) {
        int u = pq.top().second;
        Weight u_dist = pq.top().first;
        pq.pop();

        std::vector<std::pair<int, Weight>> neighbors = graph.getNeighbors(u);
        for (const auto& neighbor : neighbors) {
            int v = neighbor.first;
            Weight weight = neighbor.second;

            if (dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
                pq.push({dist[v], v});
            }
        }
    }
    //for (int i=0; i<dist.size(); i++)
        //cout<<dist[i]<<" ";
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout<<duration.count()<<endl;
    return dist;
}

void printDistances(pvector<Weight>&shortest_paths)
{

    std::cout << "Shortest paths from vertex 0:" << endl;
    for (int i = 0; i < shortest_paths.size(); ++i) {
        std::cout<<"("<<i << "--> " << shortest_paths[i]<<") ";
    }
    std::cout<<endl;
}


void comparePath(pvector<Weight> &A, pvector<Weight> &B, vector<Weight>D)
{
    if(A.size() != B.size())
    {
        printf("Size doesn't matches");
        return;
    }
    int total = 0;
    for (int i=0; i<A.size(); i++)
    {
        if((D[i]-B[i])*(D[i]-B[i]) > 0.000001)
        {
            //printf("MitchMatch Found!!!! for node %d \n",i);
            //cout<<(D[i])<<"  "<<A[i]<<" <----> "<<B[i]<<endl;
            total++;

        }
    }
    std::cout<<"\nTotal "<<total<<" Mismatch Found With Serial Dijkstra\n"<<endl;

    total = 0;

    for (int i=0; i<A.size(); i++)
    {
        if((A[i]-B[i])*(A[i]-B[i])  > 0.000001)
        {
            //printf("MitchMatch Found!!!! for node %d \n",i);
            //cout<<(D[i])<<"  "<<A[i]<<" <----> "<<B[i]<<endl;
            total++;
        }
    }
    std::cout<<"Total "<<total<<" Mismatch Found with Single thread\n"<<endl;   
}

int main(int argc, char *argv[]) {
    // Example weighted adjacency list

    global_src = 0;

    Helper util;

    AdjacencyList adjlist;



    //It's for non weighted list. Comment the line when using weighted u,v,w list
    //adjlist = util.readEdgeListFromInputFile(argc, argv);

    //It's for weighted list. Comment the line when not using weighted u,v,w list
    adjlist = util.readDirectedWeightedEdgeListFromInputFile(argc, argv);

    const char* inp = argv[1];
    const char* suffix = "_results_PDR_ADS_OPT.txt";

    // Calculate the length of the original string
    size_t inp_length = strlen(inp);
    size_t suffix_length = strlen(suffix);
    // Allocate memory for the new string
    char* new_str = new char[inp_length + suffix_length + 1];
    // Copy the original string to the new string
    strcpy(new_str, inp);
    // Concatenate the suffix to the new string
    strcat(new_str, suffix);

    std::cout<<new_str<<endl;


    freopen(new_str,"w",stdout);

    int num_vertices = adjlist.size();
    
    WCSRGraph graph(num_vertices);
    graph.fromAdjacencyList(adjlist);
    int num_edges = graph.num_edges;
    std::cout<<"(Graph Properties) : Nodes# "<<num_vertices<<"  Edges# "<<num_edges<<endl;
    std::cout<<"(Graph Properties) : Max Degree# "<<graph.max_degree<<"  Avg Degree# "<<graph.avg_degree<<endl;
    //graph.display();

    pvector<Weight> shortest_paths;
    vector<Weight> shortest_paths_0;
    pvector<Weight> shortest_paths_1;
    pvector<Weight> shortest_paths_2;

    // Testing Correctness
    int d = 1;
    if(graph.avg_degree<=4)
    {
        d=50;
    }
    shortest_paths_0 = dijkstra(graph,global_src);   
    shortest_paths_1 = delta_stepping(1, graph, global_src, d);
    shortest_paths_2 = delta_stepping(16, graph, global_src, d);
    
    comparePath(shortest_paths_1, shortest_paths_2, shortest_paths_0);
    

    std::cout<<"Running for Dijkastras"<<endl;  
    shortest_paths_0 = dijkstra(graph,global_src);
    
    int it=1;
    for (Weight del= 1 ; del<=150;)
    {

        printf("\n\n Running for Delta stepping with delta = %d\n ------->\n",del);
        shortest_paths.clear();
    
        vector<int> threads = {1,2,4,8,14,16,20,24};
        for (int p = 0; p<threads.size(); p++)
        {
            //cout<<"Running with thread : "<<threads[p]<<endl;
            int thread = threads[p];
            int totaltime=0;

            shortest_paths.clear();
            //auto start = std::chrono::high_resolution_clock::now();

            shortest_paths = delta_stepping(thread, graph, global_src, del);
                /****Code segment above*****/
            //auto end = std::chrono::high_resolution_clock::now();
            // Output the duration
            //auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            //totaltime+=duration.count();

            //std::cout << "Iteration# "<<i+1 <<" Runtime for Dijkastra: " << duration.count() << " milliseconds" << std::endl;
            //cout<<totaltime<<endl;
        }


        if(graph.avg_degree <5)
        {
            del = it*10;
            it++;
           
        }
        else{
             del+=5;
             if(del>=25)
                del+=150;
        }

        //if(del==1){del--;}
    } 
    //printDistances(shortest_paths);

}
