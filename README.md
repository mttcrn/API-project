# Algorithms and Data Structures Project 2022/2023.
The project provides a practical example of applying data structures and algorithms to solve real-world problem.

Grade obtained: 30/30.

## Summary

The goal of this project is to design and implement a system that manages an highway, including stations with electric vehicle rental services, vehicle additions, demolitions, and route planning. 
The core functionality of the project is implemented through a binary tree and the breadth-first search (BFS) algorithm for route planning.

## Features

1. **Station Management:** Allow the addition of stations at specified distances from the start of the highway. Each station has a defined number of electric vehicles, each with its own battery capacity.

2. **Route Planning:** Enable users to plan routes between two stations, with the following constraints:
   - The route should have the minimum number of stops possible.
   - Users can only rent vehicles from stations and cannot backtrack during the journey.
   - When multiple routes have the same number of stops, the system should favor routes with shorter distances from the highway's start.

3. **Vehicle Management:** Implement the ability to add and remove electric vehicles at stations.

