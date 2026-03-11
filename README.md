# AxoPlotl

There are two main versions
- AxoPlotl OpenGL (main): How it all started.
- AxoPlotl WebGPU (webgpu): Currently developed. Currently lacks the picking feature of the OpenGL, but has the most sophisticated property visualization.

Additionally, there is the polyscope branch where some code was added to Polyscope to visualize OpenVolume Meshes and some of their properties.

## Windows (not tested in a while)
```
cmake -G "MinGW Makefiles" -S . -B build
cmake --build build
cd build
start AxoPlorlApp.exe
```
