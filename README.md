# Black Hole Simulation

OpenGL-based black hole ray tracing simulation with gravitational lensing.

## Dependencies
- OpenGL 4.3+
- GLFW 3.3+
- GLEW 2.1+
- GLM 0.9.9+
- C++20 compiler
- CMake 3.28+

## Building

### Linux/Mac
```bash
mkdir build
cd build
cmake ..
make
./black_hole_simulation
```

### Windows (Visual Studio)
```bash
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022"
# Open .sln file in Visual Studio and build
```

## Controls
- **Left Click + Drag**: Rotate camera

## Project Structure
- `src/` - Source files
- `include/` - Header files
- `res/shaders/` - GLSL shaders
- `res/compute/` - Compute shaders