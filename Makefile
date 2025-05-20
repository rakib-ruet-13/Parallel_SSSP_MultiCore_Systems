# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -fopenmp

# Target executables
TARGETS = SSSP_Undirected.exe SSSP_Directed.exe PDR_SSSP_Undirected.exe PDR_SSSP_Directed.exe ADS_PDR_Undirected.exe ADS_PDR_Directed.exe Opt_ADS_PDR_Undirected.exe Opt_ADS_PDR_Directed.exe
# Default target
all: $(TARGETS)

# Compile SSSP_Undirected.exe
SSSP_Undirected.exe: SSSP_Undirected.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

# Compile SSSP_Directed.exe
SSSP_Directed.exe: SSSP_Directed.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

# Compile PDR_SSSP_Undirected.exe
PDR_SSSP_Undirected.exe: PDR_SSSP_Undirected.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

# Compile PDR_SSSP_Directed.exe
PDR_SSSP_Directed.exe: PDR_SSSP_Directed.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

# Compile ADS_PDR_SSSP_Undirected.exe
ADS_PDR_Undirected.exe: ADS_PDR_Undirected.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

# Compile ADS_PDR_SSSP_Directed.exe
ADS_PDR_Directed.exe: ADS_PDR_Directed.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

# Compile ADS_PDR_SSSP_Undirected.exe
Opt_ADS_PDR_Undirected.exe: Opt_ADS_PDR_Undirected.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

# Compile ADS_PDR_SSSP_Directed.exe
Opt_ADS_PDR_Directed.exe: Opt_ADS_PDR_Directed.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

# Clean up
clean:
	rm -f $(TARGETS)

.PHONY: all clean

