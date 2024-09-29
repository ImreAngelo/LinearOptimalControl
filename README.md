# Linear Optimal Control Solver
This solver was developed as part of a [bachelor thesis](https://hdl.handle.net/11250/3085505) in mathematics at NTNU. 
It solves linear optimal control problems by using complete parameterization.


## Usage
Currently, the solver is coupled to a graphical interace. In the future I want to make the solver a separate library. 
The graphical interface is simple to use, since it only contains a few examples and no general solving capability.

## Dependencies
- [CPLEX (Concert Technology C++ API)](https://www.ibm.com/products/ilog-cplex-optimization-studio/cplex-optimizer)
- [Dear ImGui](https://github.com/ocornut/imgui)
- [ImPlot](https://github.com/epezent/implot)
- [GLFW](https://www.glfw.org/)
- [Eigen3](https://eigen.tuxfamily.org/index.php?title=Main_Page)


## Build Instructions
Currently only tested on windows (x64) and is only set up for compiling in Visual Studio.
Build with x64 config and Debug or Release profile in Visual Studio.
If you want to build without MSVC, make sure you compile with C++17 or later.
