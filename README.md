# DMCLS: A Dual Mode Collaborative Local Search for the Min-Max Edge Crossing Problem

This project contains the experimental code for the paper **A Dual Mode Collaborative Local Search for the Min-Max Edge Crossing Problem**.

## Environment Requirements

- **Compiler**: `g++ 11` or higher
- **Standard**: C++11 or higher

## How to Run the Code

You can compile the code using the following command:

```bash
g++ -std=c++11 -O3 -o DMCLS DMCLS.cpp
```

### Configuration

Before running, please configure the following parameters and paths in the `main()` function of `DMCLS.cpp`:

```cpp
// Algorithm parameters
Short_k = 6;          // Short-range search distance control
Long_k = {1, 1};      // Long-range search distance control (numerator, denominator)
s_restore = 3;        // Snapshot recovery strength factor

// Instance folder path
const std::string target_folder = "path/to/your/instances";
```

### Execution

Run the compiled executable directly:

```bash
./DMCLS
```

The program will automatically traverse all instance files in the specified folder, and run each instance **10 times** with random seeds from 0 to 9.

### Output Format

For each run, the program outputs the result in the following format:

```
<instance_file_path>,<best_max_crossing>
```

- **Time limit**: 60 seconds per run (configured in the `solve()` function)
- **Output**: The minimum value of maximum edge crossings found

## Instance Format

```
<number of vertices> <number of edges> <number of layers>
<size of layer 1>
<size of layer 2>
...
<size of layer k>
<u1> <v1>
<u2> <v2>
...
```

### Format Description

- **First line**: Three integers representing the number of vertices, the number of edges, and the number of layers, respectively.
- **Layer size lines**: The next `k` lines each contain an integer indicating the number of vertices in the corresponding layer. Vertices are numbered sequentially from 1 to the total number of vertices, layer by layer.
- **Edge lines**: Each subsequent line represents one edge of the graph, connecting two vertices from different layers.

### Example

Take the instance `north.30.29.12` as an example. The following are the first few lines of this instance:

```
30 29 4
8
7
8
7
1 10
1 11
2 10
2 12
3 11
3 13
...
```

The first line indicates that there are 30 vertices, 29 edges, and 4 layers.
The next 4 lines indicate that the layers contain 8, 7, 8, and 7 vertices respectively.
The following lines represent the edges between vertices of different layers.

## Algorithm Overview

DMCLS (Dual Mode Collaborative Local Search) is a local search algorithm designed for solving the Min-Max Edge Crossing Problem (MMECP). It features:

- **Dual Evaluation Modes**: Two complementary evaluation strategies — the **critical mode** focusing on maximum crossing edges and the **perturbation mode** exploring broader search space — work collaboratively to balance intensification and diversification.
- **Snapshot Recovery Mechanism**: Periodically restores to the best-found solution state to escape from local optima, with adaptive recovery strength control.
- **Age-based Selection**: Incorporates an age mechanism to prefer selecting vertices that have not been moved for a long time, enhancing search diversity.
- **Efficient Delta Evaluation**: Optimized incremental update strategy for efficiently computing the change in maximum crossings after each swap operation.

## Project Structure

```
DMCLS/
├── DMCLS.cpp          # Main source file containing the algorithm implementation
├── MMACsolver9.h      # Header file with the solver class definition
└── instances/         # Test instances directory
    ├── connected/     # Connected graph instances
    ├── uniform/       # Uniform random graph instances
    ├── Rome/          # Rome library instances
    └── North/         # North library instances
```

## Contact Information

If you have any questions or suggestions, feel free to reach out to me via:

- **Email**: 778845121@qq.com
