#include<bits/stdc++.h>
#include<omp.h>
#include "global.cpp"

using namespace std;

class WCSRGraph {
public:
    int num_vertices;
    int num_edges;
    int max_degree;
    int avg_degree;
    vector<Node> row_ptr;       // Row pointers
    vector<Node> col_indices;   // Column indices
    vector<Weight> weights;    // Weights for edges


    WCSRGraph(int num_vertices) : num_vertices(num_vertices) {}

    void fromAdjacencyList(vector<vector<WNode>>& adjlist) {
        row_ptr.push_back(0); // The first row pointer is always 0
        max_degree = 0;

        for (int i = 0; i < adjlist.size(); ++i) {
            max_degree = max(max_degree, (int)adjlist[i].size());
            for (int j = 0; j < adjlist[i].size(); ++j) {
                col_indices.push_back(adjlist[i][j].first);
                weights.push_back(adjlist[i][j].second);
            }
            row_ptr.push_back(col_indices.size()); // Update the row pointer
        }
        num_edges = col_indices.size();
        avg_degree = num_edges/num_vertices;
    }

    int getNumVertices() const {
        return num_vertices;
    }

    int getNumEdges() const {
        return col_indices.size();
    }


    vector<WNode> getNeighbors(Node vertex) const {
        vector<WNode> neighbors;
        for (int i = row_ptr[vertex]; i < row_ptr[vertex + 1]; ++i) {
            neighbors.push_back({col_indices[i], weights[i]});
        }
        return neighbors;
    }


    void display() {
        cout << "CSR Representation:" << endl;
        cout << "Row pointers:";
        for (int i = 0; i < row_ptr.size(); ++i) {
            cout << " " << row_ptr[i];
        }
        cout << endl;
        cout << "Column indices:";
        for (int i = 0; i < col_indices.size(); ++i) {
            cout << " " << col_indices[i];
        }
        cout << endl;
        cout << "Weights:";
        for (int i = 0; i < weights.size(); ++i) {
            cout << " " << weights[i];
        }
        cout << endl;
    }
};





class WGraph {
public:
    int num_vertices;
    std::vector<std::vector<WNode>> adj_list; // Each element is a pair of (vertex, weight)

public:
    WGraph(int num_vertices) : num_vertices(num_vertices) {
        adj_list.resize(num_vertices+1);
    }

    void addEdge(Node src, Node dest, Weight weight) {
        // Add edge from src to dest with given weight
        adj_list[src].push_back(std::make_pair(dest, weight));
    }

    void display() {
        std::cout << "Adjacency List Representation:" << std::endl;
        for (int i = 0; i < num_vertices; ++i) {
            std::cout << i << " --> ";
            for (auto& neighbor : adj_list[i]) {
                std::cout << "(" << neighbor.first << ", " << neighbor.second << ") ";
            }
            std::cout << std::endl;
        }
    }
};

class Helper
{
    public:
    // Custom comparison function for sorting
    bool static compareEdges(std::pair<int, Weight> a, std::pair<int, Weight> b) {
        // First, compare based on vertex number
        if (a.second != b.second) {
            return a.second < b.second;
        } else {
            // If vertex numbers are equal, compare based on weight
            return a.first < b.first;
        }
    }

    void static parallelEdgeShort(std::vector< std::vector< std::pair<Node, Weight>>>& adjlist)
    {
        #pragma omp parallel for
        for (int i = 0; i < adjlist.size(); ++i) {
            sort(adjlist[i].begin(), adjlist[i].end(), compareEdges);
        }
    }

    //This function takes input from file and create the adjacency list formation with index 0. If index is given 1, it converts the index into 0

    std::vector<std::vector<WNode>> static readEdgeListFromInputFile(int argc, char *argv[])
    {
        if (argc < 2) {
            std::cout << "Usage: " << argv[0] << " provide an input file for for graph" << std::endl;
            std::vector<std::vector<WNode>> Temp;
            return Temp;
        }

        const char* input = argv[1];

        freopen(input,"r", stdin);

        std::cout<<"Starting the file.......... ";

        int u,v, minVal, maxVal;

        minVal = INT32_MAX/2;
        maxVal = 0;

        std::vector<std::pair<Node,Node>> edgeList;

        while(scanf("%d %d",&u,&v) !=EOF)
        {
            minVal = std::min(minVal,u);

            minVal = std::min(minVal,v);

            maxVal = std::max(maxVal,u);
            maxVal = std::max(maxVal,v);
            
            edgeList.push_back(std::make_pair(u,v));
            //std::cout<<u<<" -> "<<v<<"  "<<std::endl;
        }
        fclose(stdin);

        int addValue = 0;
        if(minVal != 0) addValue = -1;

        //std::cout<<minVal<<" -> "<<maxVal<<"  "<<std::endl;
        
        //std:: cout<<"Input Done"<<std::endl;

        WGraph graph(maxVal+addValue);

        srand(time(0));

        for(int i=0; i<edgeList.size(); i++)
        {
            u = edgeList[i].first;
            v = edgeList[i].second;
            graph.addEdge(u+addValue,v+addValue, (Weight)(1+rand()%1000));
            graph.addEdge(v+addValue,u+addValue, (Weight)(1+rand()%1000));
        }

        //graph.display();
        

        // modification here for PDR


        vector< pair<Node,int>> degrees;

        for (int i=0; i<graph.adj_list.size(); i++)
        {
            degrees.push_back(make_pair(i,graph.adj_list[i].size()));
        }

        
       sort(degrees.begin(), degrees.end(), [](const auto &a, const auto &b) {
        return a.second > b.second; // Sorting in descending order
        });


        map<Node, Node> modified_index;
        map<Node, Node> prev_index;

        for (int i=0; i<degrees.size(); i++)
        {
            modified_index[degrees[i].first]=i;
            prev_index[i] = degrees[i].first;
        }

        //Make the new graph
        WGraph modified_graph(maxVal+addValue);
        for (Node i=0; i<graph.adj_list.size(); i++)
        {
            for (Node j=0; j<graph.adj_list[i].size(); j++)
            {
                Node u = i;
                Node v = graph.adj_list[i][j].first;
                Weight w = graph.adj_list[i][j].second;

                modified_graph.addEdge(modified_index[u],modified_index[v],w);
            }
        }

        graph.adj_list.clear();
        parallelEdgeShort(modified_graph.adj_list);

        global_src = modified_index[0];

        //graph.display();

        return modified_graph.adj_list;
    }


    std::vector<std::vector<WNode>> static readEdgeListFromInputFileWithPrefix(int argc, char *argv[])
    {
        if (argc < 2) {
            std::cout << "Usage: " << argv[0] << " provide an input file for for graph" << std::endl;
            std::vector<std::vector<WNode>> Temp;
            return Temp;
        }

        const char* input = argv[1];

        freopen(input,"r", stdin);

        std::cout<<"Starting the file.......... "<<input<<endl;

        int u,v, minVal, maxVal;

        minVal = INT32_MAX/2;
        maxVal = 0;

        string str;
        for(int p=0; p<4; p++)
        {
            getline(cin,str);
            cout<<str<<endl;
        }
        


        std::vector<std::pair<Node,Node>> edgeList;

        while(scanf("%d %d",&u,&v) !=EOF)
        {
            minVal = std::min(minVal,u);

            minVal = std::min(minVal,v);

            maxVal = std::max(maxVal,u);
            maxVal = std::max(maxVal,v);
            
            edgeList.push_back(std::make_pair(u,v));
            //std::cout<<u<<" -> "<<v<<"  "<<std::endl;
        }
        fclose(stdin);

        int addValue = 0;
        if(minVal != 0) addValue = -1;

        //std::cout<<minVal<<" -> "<<maxVal<<"  "<<std::endl;
        
        //std:: cout<<"Input Done"<<std::endl;

        WGraph graph(maxVal+addValue);

        srand(time(0));

        for(int i=0; i<edgeList.size(); i++)
        {
            u = edgeList[i].first;
            v = edgeList[i].second;
            Weight w = (Weight)(1+rand()%1000);
            graph.addEdge(u+addValue,v+addValue, w );
            graph.addEdge(v+addValue,u+addValue, w );;
        }

        //graph.display();

        // modification here for PDR


        vector< pair<Node,int>> degrees;

        for (int i=0; i<graph.adj_list.size(); i++)
        {
            degrees.push_back(make_pair(i,graph.adj_list[i].size()));
        }

        
       sort(degrees.begin(), degrees.end(), [](const auto &a, const auto &b) {
        return a.second > b.second; // Sorting in descending order
        });


        map<Node, Node> modified_index;
        map<Node, Node> prev_index;

        for (int i=0; i<degrees.size(); i++)
        {
            modified_index[degrees[i].first]=i;
            prev_index[i] = degrees[i].first;
        }

        //Make the new graph
        WGraph modified_graph(maxVal+addValue);
        for (Node i=0; i<graph.adj_list.size(); i++)
        {
            for (Node j=0; j<graph.adj_list[i].size(); j++)
            {
                Node u = i;
                Node v = graph.adj_list[i][j].first;
                Weight w = graph.adj_list[i][j].second;

                modified_graph.addEdge(modified_index[u],modified_index[v],w);
            }
        }

        graph.adj_list.clear();

        global_src = modified_index[0];

        parallelEdgeShort(modified_graph.adj_list);

        //graph.display();

        return modified_graph.adj_list;


    }

    std::vector<std::vector<WNode>> static readDirectedEdgeListFromInputFileWithPrefix(int argc, char *argv[])
    {
        if (argc < 2) {
            std::cout << "Usage: " << argv[0] << " provide an input file for for graph" << std::endl;
            std::vector<std::vector<WNode>> Temp;
            return Temp;
        }

        const char* input = argv[1];

        freopen(input,"r", stdin);
        

        std::cout<<"Starting the file.......... "<<input<<endl;

        int u,v, minVal, maxVal;

        minVal = INT32_MAX/2;
        maxVal = 0;

        string str;
        for(int p=0; p<4; p++)
        {
            getline(cin,str);
            cout<<str<<endl;
        }
        


        std::vector<std::pair<Node,Node>> edgeList;

        while(scanf("%d %d",&u,&v) !=EOF)
        {
            minVal = std::min(minVal,u);
            minVal = std::min(minVal,v);

            maxVal = std::max(maxVal,u);
            maxVal = std::max(maxVal,v);
            
            edgeList.push_back(std::make_pair(u,v));
            //std::cout<<u<<" -> "<<v<<"  "<<std::endl;
        }
        fclose(stdin);

        int addValue = 0;
        if(minVal != 0) addValue = -1;

        //std::cout<<minVal<<" -> "<<maxVal<<"  "<<std::endl;
        
        //std:: cout<<"Input Done"<<std::endl;

        WGraph graph(maxVal+addValue);

        srand(time(0));

        for(int i=0; i<edgeList.size(); i++)
        {
            u = edgeList[i].first;
            v = edgeList[i].second;
            Weight w = (Weight)(1+rand()%1000);
            graph.addEdge(u+addValue,v+addValue, w);
            
        }

        //graph.display();

        vector< pair<Node,int>> degrees;

        for (int i=0; i<graph.adj_list.size(); i++)
        {
            degrees.push_back(make_pair(i,graph.adj_list[i].size()));
        }

        
       sort(degrees.begin(), degrees.end(), [](const auto &a, const auto &b) {
        return a.second > b.second; // Sorting in descending order
        });


        map<Node, Node> modified_index;
        map<Node, Node> prev_index;

        for (int i=0; i<degrees.size(); i++)
        {
            modified_index[degrees[i].first]=i;
            prev_index[i] = degrees[i].first;
        }

        //Make the new graph
        WGraph modified_graph(maxVal+addValue);
        for (Node i=0; i<graph.adj_list.size(); i++)
        {
            for (Node j=0; j<graph.adj_list[i].size(); j++)
            {
                Node u = i;
                Node v = graph.adj_list[i][j].first;
                Weight w = graph.adj_list[i][j].second;

                modified_graph.addEdge(modified_index[u],modified_index[v],w);
            }
        }

        graph.adj_list.clear();

        global_src = modified_index[0];
        parallelEdgeShort(modified_graph.adj_list);

        //graph.display();

        return modified_graph.adj_list;
    }


    std::vector<std::vector<WNode>> static readWeightedEdgeListFromInputFile(int argc, char *argv[])
    {
        if (argc < 2) {
            std::cout << "Usage: " << argv[0] << " provide an input file for for graph" << std::endl;
            std::vector<std::vector<WNode>> Temp;
            return Temp;
        }

        const char* input = argv[1];

        freopen(input,"r", stdin);

        std::cout<<"Starting the file.......... ";

        int u,v, minVal, maxVal;
        double w;
        minVal = INT32_MAX/2;
        maxVal = 0;

        std::vector<std::pair<Node,WNode>> WedgeList;

        while(scanf("%d %d %lf",&u,&v,&w) !=EOF)
        {
            minVal = std::min(minVal,u);

            minVal = std::min(minVal,v);

            maxVal = std::max(maxVal,u);
            maxVal = std::max(maxVal,v);
            int weight = 1+ floor(w*1000.0);
            WedgeList.push_back(std::make_pair(u,make_pair(v,weight)));
            //std::cout<<u<<" -> "<<v<<"  "<<std::endl;
        }
        fclose(stdin);

        int addValue = 0;
        if(minVal != 0) addValue = -1;

        //std::cout<<minVal<<" -> "<<maxVal<<"  "<<std::endl;
        
        //std:: cout<<"Input Done"<<std::endl;

        WGraph graph(maxVal+addValue);

        srand(time(0));

        for(int i=0; i<WedgeList.size(); i++)
        {
            u = WedgeList[i].first;
            v = WedgeList[i].second.first;
            w = WedgeList[i].second.second;
            graph.addEdge(u+addValue,v+addValue, w);
            graph.addEdge(v+addValue,u+addValue, w);
        }

        //graph.display();

        vector< pair<Node,int>> degrees;

        for (int i=0; i<graph.adj_list.size(); i++)
        {
            degrees.push_back(make_pair(i,graph.adj_list[i].size()));
        }

        
       sort(degrees.begin(), degrees.end(), [](const auto &a, const auto &b) {
        return a.second > b.second; // Sorting in descending order
        });


        map<Node, Node> modified_index;
        map<Node, Node> prev_index;

        for (int i=0; i<degrees.size(); i++)
        {
            modified_index[degrees[i].first]=i;
            prev_index[i] = degrees[i].first;
        }

        //Make the new graph
        WGraph modified_graph(maxVal+addValue);
        for (Node i=0; i<graph.adj_list.size(); i++)
        {
            for (Node j=0; j<graph.adj_list[i].size(); j++)
            {
                Node u = i;
                Node v = graph.adj_list[i][j].first;
                Weight w = graph.adj_list[i][j].second;

                modified_graph.addEdge(modified_index[u],modified_index[v],w);
            }
        }

        graph.adj_list.clear();
        global_src = modified_index[0];

        parallelEdgeShort(modified_graph.adj_list);

        //graph.display();

        return modified_graph.adj_list;

    }


    std::vector<std::vector<WNode>> static readDirectedWeightedEdgeListFromInputFile(int argc, char *argv[])
    {
        if (argc < 2) {
            std::cout << "Usage: " << argv[0] << " provide an input file for for graph" << std::endl;
            std::vector<std::vector<WNode>> Temp;
            return Temp;
        }

        const char* input = argv[1];

        freopen(input,"r", stdin);

        std::cout<<"Starting the file.......... ";

        int u,v, minVal, maxVal;
        double w;

        minVal = INT32_MAX/2;
        maxVal = 0;

        std::vector<std::pair<Node,WNode>> WedgeList;

        while(scanf("%d %d %lf",&u,&v,&w) !=EOF)
        {
            minVal = std::min(minVal,u);

            minVal = std::min(minVal,v);

            maxVal = std::max(maxVal,u);
            maxVal = std::max(maxVal,v);
            
            int weight = 1+ floor(w*1000.0);
            WedgeList.push_back(std::make_pair(u,make_pair(v,weight)));
            //std::cout<<u<<" -> "<<v<<"  "<<std::endl;
        }
        fclose(stdin);

        int addValue = 0;
        if(minVal != 0) addValue = -1;

        //std::cout<<minVal<<" -> "<<maxVal<<"  "<<std::endl;
        
        //std:: cout<<"Input Done"<<std::endl;

        WGraph graph(maxVal+addValue);

        srand(time(0));

        for(int i=0; i<WedgeList.size(); i++)
        {
            u = WedgeList[i].first;
            v = WedgeList[i].second.first;
            Weight w = WedgeList[i].second.second;

            int weight = w;

            graph.addEdge(u+addValue,v+addValue, weight);
            //graph.addEdge(v+addValue,u+addValue, w);
        }

        //graph.display();

        vector< pair<Node,int>> degrees;

        for (int i=0; i<graph.adj_list.size(); i++)
        {
            degrees.push_back(make_pair(i,graph.adj_list[i].size()));
        }

        
       sort(degrees.begin(), degrees.end(), [](const auto &a, const auto &b) {
        return a.second > b.second; // Sorting in descending order
        });


        map<Node, Node> modified_index;
        map<Node, Node> prev_index;

        for (int i=0; i<degrees.size(); i++)
        {
            modified_index[degrees[i].first]=i;
            prev_index[i] = degrees[i].first;
        }

        //Make the new graph
        WGraph modified_graph(maxVal+addValue);
        for (Node i=0; i<graph.adj_list.size(); i++)
        {
            for (Node j=0; j<graph.adj_list[i].size(); j++)
            {
                Node u = i;
                Node v = graph.adj_list[i][j].first;
                Weight w = graph.adj_list[i][j].second;

                modified_graph.addEdge(modified_index[u],modified_index[v],w);
            }
        }

        graph.adj_list.clear();
        global_src = modified_index[0];

        parallelEdgeShort(modified_graph.adj_list);

        //graph.display();

        return modified_graph.adj_list;

    }
};