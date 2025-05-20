#!/bin/bash

# List of arguments to pass to the program
args_Dir=(
    #"Inputs/Dataset/Directed/WikiTalk_graph_dir.txt"
    #"Inputs/Dataset/Directed/cit-patent_graph_dir.txt"
    "Inputs/Dataset/Directed/soc-twitter-2010_dir.txt"
    "Inputs/Dataset/Directed/roadNet-CA_graph_Dir.txt"
    # Add more arguments as needed
)

args_Undir=(
    #"Inputs/Dataset/Undirected/bio-human-gene1_graph_ud.txt"
    #"Inputs/Dataset/Undirected/com-amazon_ungraph.txt"
    #"Inputs/Dataset/Undirected/com-dblp_ungraph..txt"
    "Inputs/Dataset/Undirected/com-lj_ungraph.txt"
    "Inputs/Dataset/Undirected/com-orkut_ungraph.txt"
    "Inputs/Dataset/Undirected/com-youtube_ungraph.txt"
    "Inputs/Dataset/Undirected/graph500_kron_20_ud.txt"
    "Inputs/Dataset/Undirected/graph500_kron_21_ud.txt"
    "Inputs/Dataset/Undirected/graph500_kron_22_ud.txt"
    "Inputs/Dataset/Undirected/graph500_kron_23_ud.txt"
    #"Inputs/Dataset/Undirected/road-asia-osm_ud.txt"
    "Inputs/Dataset/Undirected/road-road-usa_ud.txt"
    "Inputs/Dataset/Undirected/socfb-uci-uni_ud.txt"
    "Inputs/Dataset/Undirected/soc-sinaweibo_ud.txt"
    # Add more arguments as needed
)



# Path to your C++ program
program_Dir="./Opt_ADS_PDR_Directed.exe"
program_Udir="./Opt_ADS_PDR_Undirected.exe"

echo "Running OPT ADS PDR_DS SSSP Undirected Networks: --------------------------> "
# Loop through each argument and run the program
for arg in "${args_Undir[@]}"; do
    echo "Running with argument: $arg"
    $program_Udir $arg
    echo "--------------------------------"
    echo "--------------------------------"
    echo "--------------------------------"
done

echo "Running OPT ADS PDR_SSSP for Directed Networks: -------> "
# Loop through each argument and run the program
for arg1 in "${args_Dir[@]}"; do
    echo "Running with argument: $arg1"
    $program_Dir $arg1
    echo "--------------------------------"
    echo "--------------------------------"
    echo "--------------------------------"
done



