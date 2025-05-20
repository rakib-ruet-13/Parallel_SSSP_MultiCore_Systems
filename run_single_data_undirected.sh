#!/bin/bash

# List of arguments to pass to the program

args_Undir=(
    #"Inputs/Dataset/Undirected/bio-human-gene1_graph_ud.txt"
    #"Inputs/Dataset/Undirected/com-amazon_ungraph.txt"
    #"Inputs/Dataset/Undirected/com-dblp_ungraph..txt"
    "Inputs/Dataset/Undirected/com-lj_ungraph.txt"
    "Inputs/Dataset/Undirected/com-orkut_ungraph.txt"
    "Inputs/Dataset/Undirected/com-youtube_ungraph.txt"
    #"Inputs/Dataset/Undirected/graph500_kron_20_ud.txt"
    #"Inputs/Dataset/Undirected/graph500_kron_21_ud.txt"
    #"Inputs/Dataset/Undirected/graph500_kron_22_ud.txt"
    #"Inputs/Dataset/Undirected/graph500_kron_23_ud.txt"
    #"Inputs/Dataset/Undirected/road-asia-osm_ud.txt"
    "Inputs/Dataset/Undirected/road-road-usa_ud.txt"
    #"Inputs/Dataset/Undirected/socfb-uci-uni_ud.txt"
    #"Inputs/Dataset/Undirected/soc-sinaweibo_ud.txt"
    #"Inputs/Dataset/Undirected/graph500_kron_24_ud.txt"
    # Add more arguments as needed
)

# List of arguments to pass to the program
args_Dir=(
    #"Inputs/Dataset/Directed/WikiTalk_graph_dir.txt"
    #"Inputs/Dataset/Directed/cit-patent_graph_dir.txt"
    #"Inputs/Dataset/Directed/soc-twitter-2010_dir.txt"
    "Inputs/Dataset/Directed/roadNet-CA_graph_Dir.txt"
    # Add more arguments as needed
)


# Path to your C++ program
program1_Udir="./SSSP_Undirected.exe"
program2_Udir="./PDR_SSSP_Undirected.exe"
program3_Udir="./ADS_PDR_Undirected.exe"
program4_Udir="./Opt_ADS_PDR_Undirected.exe"

program1_Dir="./SSSP_Directed.exe"
program2_Dir="./PDR_SSSP_Directed.exe"
program3_Dir="./ADS_PDR_Directed.exe"
program4_Dir="./Opt_ADS_PDR_Directed.exe"

echo "Running SSSP for Undirected Networks: --------------------------> "
# Loop through each argument and run the program
for arg in "${args_Undir[@]}"; do
    echo "SSSP Base Running with argument: $arg"
    $program1_Udir $arg
    echo "--------------------------------"
    echo "PDR SSSP Running with argument: $arg"
    $program2_Udir $arg
    echo "--------------------------------"
    echo "ADS PDR Running with argument: $arg"
    $program3_Udir $arg
    echo "--------------------------------"
    echo "OPT ADS PDR Running with argument: $arg"
    $program4_Udir $arg
done


echo "Running SSSP for Directed Networks: -------> "
# Loop through each argument and run the program
for arg1 in "${args_Dir[@]}"; do
    echo "SSSP Base Running with argument: $arg1"
    $program1_Dir $arg1
    echo "--------------------------------"
    echo "PDR SSSP Running with argument: $arg1"
    $program2_Dir $arg1
    echo "--------------------------------"
    echo "ADS PDR SSSP Running with argument: $arg1"
    $program3_Dir $arg1
    echo "--------------------------------"
    echo "ADS PDR SSSP Running with argument: $arg1"
    $program4_Dir $arg1
    echo "--------------------------------"
done
