# An Efficient Multi-core Parallel Implementation of SSSP Algorithm with Decreasing Delta-stepping

## Overview of the Project
This project contains an efficient multi-core parallel implementation of the Single-Source Shortest Path (SSSP) algorithm using a dynamic Delta-Stepping approach, termed PD3. It addresses the challenges of scalability and load imbalance in large-scale graph processing on shared-memory systems. Key optimizations include adaptive delta reduction, property-driven graph reordering, bucket fusion, and dynamic load balancing. The algorithm is evaluated on both synthetic and real-world datasets, showing substantial performance gains over serial and parallel baselines. Results demonstrate up to 65× speedup over Dijkstra’s algorithm and a 1.4× improvement over GAPBS’s parallel SSSP.

## Key Contributions
* Introduces PD3, an adaptive parallel Delta-Stepping algorithm with novel optimizations to enhance efficiency and scalability.

* Implements techniques such as graph reordering, bucket fusion, and dynamic load balancing to improve performance on power-law distributed graphs.

* Provides a comprehensive comparative analysis against Dijkstra, Bellman-Ford, and GAPBS’s parallel SSSP.

## Technology Used
* OpenMP for shared-memory parallelization
* Compressed Sparse Row (CSR) format for graph representation
* C++ implementation on multi-core CPUs

## Dataset Used
<table border="1" cellpadding="5" cellspacing="0">
  <thead>
    <tr>
      <th>Dataset</th>
      <th>#Vertices</th>
      <th>#Edges</th>
      <th>Max Degree</th>
      <th>Avg Degree</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>Kron 20</td>
      <td>1,048,576</td>
      <td>33,554,432</td>
      <td>138344</td>
      <td>32</td>
    </tr>
    <tr>
      <td>Kron 21</td>
      <td>2,097,152</td>
      <td>67,108,864</td>
      <td>210643</td>
      <td>32</td>
    </tr>
    <tr>
      <td>Kron 22</td>
      <td>4,194,304</td>
      <td>134,217,728</td>
      <td>320556</td>
      <td>32</td>
    </tr>
    <tr>
      <td>Kron 23</td>
      <td>8,388,608</td>
      <td>268,435,456</td>
      <td>487144</td>
      <td>32</td>
    </tr>
    <tr>
      <td>road-road-usa</td>
      <td>23,947,347</td>
      <td>57,708,624</td>
      <td>9</td>
      <td>2</td>
    </tr>
    <tr>
      <td>Live Journal</td>
      <td>4,036,538</td>
      <td>69,362,378</td>
      <td>14815</td>
      <td>17</td>
    </tr>
    <tr>
      <td>Orkut</td>
      <td>3,072,626</td>
      <td>234,370,166</td>
      <td>33313</td>
      <td>76</td>
    </tr>
    <tr>
      <td>Youtube</td>
      <td>1,157,827</td>
      <td>5,975,248</td>
      <td>28754</td>
      <td>5</td>
    </tr>
    <tr>
      <td>socfb-uci-uni</td>
      <td>58,790,782</td>
      <td>184,416,390</td>
      <td>4960</td>
      <td>3</td>
    </tr>
    <tr>
      <td>soc-sinaweibo</td>
      <td>58,655,849</td>
      <td>522,642,142</td>
      <td>278491</td>
      <td>8</td>
    </tr>
    <tr>
      <td>soc-twitter-2010</td>
      <td>21,297,772</td>
      <td>397,538,714</td>
      <td>132512905</td>
      <td>18</td>
    </tr>
  </tbody>
</table>


## Results
<table border="1" cellpadding="5" cellspacing="0">
  <caption><strong>COMPARATIVE RUNTIME (MS) OF DIFFERENT SSSP IMPLEMENTATIONS</strong></caption>
  <thead>
    <tr>
      <th rowspan="2">Graphs</th>
      <th colspan="1">Serial</th>
      <th colspan="3">Parallel</th>
    </tr>
    <tr>
      <th>Dijkstra</th>
      <th>Bellman-Ford</th>
      <th>GAPBS</th>
      <th>PD3</th>
    </tr>
  </thead>
  <tbody>
    <tr><td>Kron 20</td><td>1337</td><td>1134</td><td>54</td><td>48</td></tr>
    <tr><td>Kron 21</td><td>2857</td><td>1979</td><td>94</td><td>82</td></tr>
    <tr><td>Kron 22</td><td>6495</td><td>4765</td><td>180</td><td>144</td></tr>
    <tr><td>Kron 23</td><td>15032</td><td>11305</td><td>430</td><td>307</td></tr>
    <tr><td>road-road-usa</td><td>11991</td><td>751968</td><td>545</td><td>547</td></tr>
    <tr><td>Live Journal</td><td>8347</td><td>7769</td><td>165</td><td>124</td></tr>
    <tr><td>Orkut</td><td>11897</td><td>13322</td><td>230</td><td>223</td></tr>
    <tr><td>Youtube</td><td>1066</td><td>496</td><td>19</td><td>20</td></tr>
    <tr><td>socfb-uci-uni</td><td>92732</td><td>59112</td><td>1409</td><td>1396</td></tr>
    <tr><td>soc-sinaweibo</td><td>130333</td><td>95591</td><td>2478</td><td>2353</td></tr>
    <tr><td>soc-twitter-2010</td><td>22822</td><td>9472</td><td>2117</td><td>1749</td></tr>
  </tbody>
</table>

