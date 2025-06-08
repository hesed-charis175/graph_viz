# Graph Visualizer

Graph Visualizer is a real-time visualization tool built in C++ with ImGui, designed to help users interactively explore graph structures, modify parameters, and test pathfinding algorithms. It provides a map-based interface where users can adjust the nature of node connectivity, set start and end points, and analyze algorithm performance.

## Features
- **Map-Based Graph Representation**  
  - City-like, island-based, or maze-like graph structures  
  - Configurable node connectivity settings  
  - Click to set start, end, or intermediate nodes dynamically  
  - If a new end point is assigned, the previous end becomes an intermediate node  

- **Customizable Appearance**  
  - Adjust colors for nodes, edges, solution paths, and background  
  - Highlight key points such as start and end nodes  

- **Algorithm Testing and Logs**  
  - Supports BFS, DFS, A*, RRT, UCS, and Greedy (Eulerian distance-based) algorithms  
  - Execution time tracking to analyze algorithm efficiency  

## Core Functionalities
- Parameter tuning  
- Algorithm testing  
- Real-time pathfinding visualization  

## Technologies Used
- C++  
- ImGui  
- Graph Theory and Pathfinding Algorithms  

## Installation and Usage
1. Clone the repository  
   ```sh
   git clone https://github.com/yourusername/graph-visualizer.git
   cd graph-visualizer
