# Black Hole Simulation

OpenGL-based black hole ray tracing simulation with gravitational lensing.

## Requirements
- OpenGL 4.3+
- GLFW 3.3+
- GLEW 2.1+
- GLM 0.9.9+
- C++20 compiler
- CMake 3.28+

## Installation

### Windows
1. Install [vcpkg](https://github.com/microsoft/vcpkg) if you haven't already
2. Install dependencies:
```bash
vcpkg install glfw3 glew glm
```

### Linux (Ubuntu/Debian)
```bash
sudo apt update
sudo apt install build-essential cmake libglfw3-dev libglew-dev libglm-dev
```

### Linux (Fedora)
```bash
sudo dnf install cmake gcc-c++ glfw-devel glew-devel glm-devel
```

### macOS
```bash
brew install cmake glfw glew glm
```

## Building

### Linux/macOS
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
cmake .. -G "Visual Studio 17 2022" -DCMAKE_TOOLCHAIN_FILE=[path to vcpkg]/scripts/buildsystems/vcpkg.cmake
```
Then open the generated `.sln` file in Visual Studio and build, or build from command line:
```bash
cmake --build . --config Release
.\Release\black_hole_simulation.exe
```

## Controls
- **Left Click + Drag**: Rotate camera

## Project Structure
- `src/` - Source files
- `include/` - Header files
- `res/shaders/` - GLSL vertex and fragment shaders
- `res/compute/` - Compute shaders for ray tracing
- `res/textures/` - Texture assets (planets, noise)
- `res/planets/` - Planet configuration files
- `external/stb_image/` - Image loading library