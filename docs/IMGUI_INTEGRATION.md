# ImGui Integration Guide

This guide covers adding ImGui to the black hole simulation project for creating debug UI and settings controls.

---

## Part 1: Adding ImGui to the Project

### Step 1: Copy Required Files

From your cloned ImGui repo, copy these files into `external/imgui/`:

**Core files (from root):**
```
imgui.h
imgui.cpp
imgui_draw.cpp
imgui_tables.cpp
imgui_widgets.cpp
imgui_internal.h
imconfig.h
imstb_rectpack.h
imstb_textedit.h
imstb_truetype.h
```

**Backend files (from backends/):**
```
imgui_impl_glfw.h
imgui_impl_glfw.cpp
imgui_impl_opengl3.h
imgui_impl_opengl3.cpp
imgui_impl_opengl3_loader.h
```

**Final structure:**
```
external/
  imgui/
    imgui.h
    imgui.cpp
    imgui_draw.cpp
    imgui_tables.cpp
    imgui_widgets.cpp
    imgui_internal.h
    imconfig.h
    imstb_rectpack.h
    imstb_textedit.h
    imstb_truetype.h
    imgui_impl_glfw.h
    imgui_impl_glfw.cpp
    imgui_impl_opengl3.h
    imgui_impl_opengl3.cpp
    imgui_impl_opengl3_loader.h
```

### Step 2: Update CMakeLists.txt

Add the ImGui source files and include directory:

```cmake
cmake_minimum_required(VERSION 3.28.3)
project(black_hole_simulation)

set(CMAKE_CXX_STANDARD 20)

find_package(glm CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

# setting up the dependencies
set(DEPS
        glfw
        GLEW::GLEW
        glm::glm
        OpenGL::GL
)

# ImGui sources
set(IMGUI_SOURCES
        external/imgui/imgui.cpp
        external/imgui/imgui_draw.cpp
        external/imgui/imgui_tables.cpp
        external/imgui/imgui_widgets.cpp
        external/imgui/imgui_impl_glfw.cpp
        external/imgui/imgui_impl_opengl3.cpp
)

# also setting up the source files
set(SOURCES
        src/main.cpp
        src/Camera.cpp
        src/ComputeShader.cpp
        src/InputHandler.cpp
        src/RayTraceEngine.cpp
        src/Renderer.cpp
        src/Settings.cpp
        src/Shader.cpp
        src/Simulation.cpp
        src/Time.cpp
        src/Utils.cpp
        src/Window.cpp
        external/stb_image/stb_impl.cpp
        ${IMGUI_SOURCES}
)

add_executable(black_hole_simulation ${SOURCES})
target_link_libraries(black_hole_simulation PRIVATE ${DEPS})

# adding our include folder
target_include_directories(black_hole_simulation PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/external/stb_image
        ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui
)
```

### Step 3: Rebuild CMake

```bash
cd build
cmake ..
```

---

## Part 2: Basic ImGui Usage Pattern

### Initialization (once at startup)

```cpp
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// After creating your GLFW window and GL context:
IMGUI_CHECKVERSION();
ImGui::CreateContext();
ImGuiIO& io = ImGui::GetIO();

// Optional: Enable keyboard navigation, docking, etc.
io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

// Initialize backends
ImGui_ImplGlfw_InitForOpenGL(window, true);  // true = install callbacks
ImGui_ImplOpenGL3_Init("#version 430");       // match your GL version

// Optional: Set style
ImGui::StyleColorsDark();  // or StyleColorsLight(), StyleColorsClassic()
```

### Per-Frame Usage (in your main loop)

```cpp
// Start of frame (before your game logic)
ImGui_ImplOpenGL3_NewFrame();
ImGui_ImplGlfw_NewFrame();
ImGui::NewFrame();

// Check if ImGui wants input (for gating camera controls)
bool imguiWantsInput = ImGui::GetIO().WantCaptureMouse;

// ... your game logic, camera updates (skip if imguiWantsInput) ...

// Build your UI (can be anywhere before Render)
ImGui::Begin("Settings");
// ... widgets ...
ImGui::End();

// ... your rendering (simulation, renderer->draw()) ...

// End of frame (after your rendering, before swap buffers)
ImGui::Render();
ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

glfwSwapBuffers(window);
```

### Shutdown (at program exit)

```cpp
ImGui_ImplOpenGL3_Shutdown();
ImGui_ImplGlfw_Shutdown();
ImGui::DestroyContext();
```

---

## Part 3: Widgets You'll Need

### Windows and Layout

```cpp
// Basic window
ImGui::Begin("Window Title");
// ... widgets go here ...
ImGui::End();

// Collapsing header (foldable section)
if (ImGui::CollapsingHeader("Camera Settings")) {
    // widgets inside the collapsible section
}

// Tree node (nested collapsible)
if (ImGui::TreeNode("Advanced")) {
    // nested content
    ImGui::TreePop();  // must call this to close
}

// Separator line
ImGui::Separator();

// Same line (put next widget on same line)
ImGui::SameLine();

// Text
ImGui::Text("Hello, world!");
ImGui::TextColored(ImVec4(1,0,0,1), "Red text");
```

### Value Editors

```cpp
// Float slider
static float myFloat = 0.5f;
ImGui::SliderFloat("Float Slider", &myFloat, 0.0f, 1.0f);

// Float slider with format
ImGui::SliderFloat("Opacity", &opacity, 0.0f, 1.0f, "%.2f");

// Int slider
static int myInt = 50;
ImGui::SliderInt("Int Slider", &myInt, 0, 100);

// Drag (no min/max, drag to adjust)
ImGui::DragFloat("Drag Float", &myFloat, 0.01f);  // 0.01 = speed
ImGui::DragFloat("With Range", &myFloat, 0.01f, 0.0f, 10.0f);

// Input field (type a number)
ImGui::InputFloat("Input", &myFloat);
ImGui::InputInt("Int Input", &myInt);

// Vec3 (for positions, colors, etc.)
static float vec3[3] = {0.0f, 0.0f, 0.0f};
ImGui::SliderFloat3("Position", vec3, -10.0f, 10.0f);
ImGui::DragFloat3("Drag Vec3", vec3, 0.1f);

// Color picker
static float color[4] = {1.0f, 0.8f, 0.3f, 1.0f};
ImGui::ColorEdit4("Hot Color", color);
ImGui::ColorEdit3("RGB Only", color);  // no alpha
```

### Checkboxes and Radio Buttons

```cpp
// Checkbox
static bool enabled = true;
ImGui::Checkbox("Enable Feature", &enabled);

// Radio buttons (one of many)
static int mode = 0;
ImGui::RadioButton("Mode A", &mode, 0); ImGui::SameLine();
ImGui::RadioButton("Mode B", &mode, 1); ImGui::SameLine();
ImGui::RadioButton("Mode C", &mode, 2);
```

### Dropdowns (Combo boxes)

```cpp
// Simple combo
static int currentItem = 0;
const char* items[] = { "Flat Plane", "Incline Volume", "Raymarched" };
ImGui::Combo("Render Mode", &currentItem, items, IM_ARRAYSIZE(items));

// Combo with custom display
if (ImGui::BeginCombo("Select", items[currentItem])) {
    for (int i = 0; i < IM_ARRAYSIZE(items); i++) {
        bool isSelected = (currentItem == i);
        if (ImGui::Selectable(items[i], isSelected)) {
            currentItem = i;
        }
        if (isSelected) {
            ImGui::SetItemDefaultFocus();
        }
    }
    ImGui::EndCombo();
}
```

### Buttons

```cpp
if (ImGui::Button("Reset Camera")) {
    // button was clicked
    camera->reset();
}

// Button with size
if (ImGui::Button("Big Button", ImVec2(200, 50))) {
    // clicked
}

// Small button (no frame padding)
ImGui::SmallButton("small");
```

### Tooltips

```cpp
ImGui::SliderFloat("Complex Setting", &value, 0.0f, 1.0f);
if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("This controls the frobulation coefficient");
}

// Or simpler helper pattern
ImGui::SliderFloat("Setting", &value, 0.0f, 1.0f);
ImGui::SameLine();
ImGui::TextDisabled("(?)");
if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("Explanation here");
}
```

### Read-Only Display

```cpp
// Display values (not editable)
ImGui::Text("FPS: %.1f", fps);
ImGui::Text("Camera Pos: (%.2f, %.2f, %.2f)", pos.x, pos.y, pos.z);

// Labeled value with copy-able text
ImGui::LabelText("Label", "%.3f", someValue);

// Progress bar
ImGui::ProgressBar(0.7f, ImVec2(-1, 0), "70%");
```

---

## Part 4: Example Settings Panel for Your Project

Here's an example of what your settings UI might look like:

```cpp
void renderSettingsUI(Settings& settings, Camera& camera) {
    ImGui::Begin("Simulation Settings");

    // === Camera Section ===
    if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
        float orbitSens = settings.GetOrbitSensitivity();
        if (ImGui::SliderFloat("Orbit Sensitivity", &orbitSens, 0.0001f, 0.01f, "%.4f")) {
            settings.SetOrbitSensitivity(orbitSens);
        }

        float zoomSens = settings.GetZoomSensitivity();
        if (ImGui::SliderFloat("Zoom Sensitivity", &zoomSens, 0.001f, 0.1f, "%.3f")) {
            settings.SetZoomSensitivity(zoomSens);
        }

        // Read-only camera info
        ImGui::Separator();
        glm::vec3 pos = camera.GetPosition();
        ImGui::Text("Position: (%.1f, %.1f, %.1f)", pos.x, pos.y, pos.z);
        ImGui::Text("Radius: %.1f", camera.GetPositionSpherical().x);
    }

    // === Simulation Section ===
    if (ImGui::CollapsingHeader("Simulation")) {
        int maxSteps = settings.GetMaxRaySteps();
        if (ImGui::SliderInt("Max Ray Steps", &maxSteps, 100, 20000)) {
            settings.SetMaxRaySteps(maxSteps);
        }

        float stepSize = settings.GetRayStepSize();
        if (ImGui::SliderFloat("Ray Step Size", &stepSize, 0.1f, 5.0f)) {
            settings.SetRayStepSize(stepSize);
        }
    }

    // === Accretion Disk Section ===
    if (ImGui::CollapsingHeader("Accretion Disk")) {
        // Render mode dropdown
        static const char* modes[] = { "Flat Plane", "Incline Volume", "Raymarched" };
        int mode = settings.GetDiskRenderMode();
        if (ImGui::Combo("Render Mode", &mode, modes, IM_ARRAYSIZE(modes))) {
            settings.SetDiskRenderMode(mode);
        }

        // Disk radii
        float innerR = settings.GetDiskInnerRadiusRatio();
        float outerR = settings.GetDiskOuterRadiusRatio();
        if (ImGui::DragFloatRange2("Disk Radii (Rs)", &innerR, &outerR, 0.1f, 1.5f, 20.0f)) {
            settings.SetDiskRadii(innerR, outerR);
        }

        // Colors
        glm::vec4 colorHot = settings.GetDiskColorHot();
        glm::vec4 colorCool = settings.GetDiskColorCool();
        if (ImGui::ColorEdit4("Hot Color", &colorHot.x)) {
            settings.SetDiskColors(colorCool, colorHot);
        }
        if (ImGui::ColorEdit4("Cool Color", &colorCool.x)) {
            settings.SetDiskColors(colorCool, colorHot);
        }
    }

    // === Radial Mesh Section ===
    if (ImGui::CollapsingHeader("Radial Mesh")) {
        bool enabled = settings.IsRadialMeshEnabled();
        if (ImGui::Checkbox("Show Mesh", &enabled)) {
            settings.SetRadialMeshEnabled(enabled);
        }

        if (enabled) {
            float opacity = settings.GetRadialMeshOpacity();
            if (ImGui::SliderFloat("Opacity", &opacity, 0.0f, 1.0f)) {
                settings.SetRadialMeshOpacity(opacity);
            }

            int rings = settings.GetRadialMeshRings();
            int spokes = settings.GetRadialMeshSpokes();
            bool changed = false;
            changed |= ImGui::SliderInt("Rings", &rings, 5, 100);
            changed |= ImGui::SliderInt("Spokes", &spokes, 8, 72);
            if (changed) {
                settings.SetRadialMeshResolution(rings, spokes);
            }
        }
    }

    ImGui::End();
}
```

---

## Part 5: Input Gating with Your Architecture

When ImGui is active (mouse over a panel), you want to block input from reaching the camera. Here's how it fits with your InputState approach:

```cpp
// In your main loop / Application::run()

// Start ImGui frame
ImGui_ImplOpenGL3_NewFrame();
ImGui_ImplGlfw_NewFrame();
ImGui::NewFrame();

// Build UI
renderSettingsUI(settings, camera);

// Check if ImGui wants the input
bool imguiWantsMouse = ImGui::GetIO().WantCaptureMouse;
bool imguiWantsKeyboard = ImGui::GetIO().WantCaptureKeyboard;

// Only update camera if ImGui doesn't want input
if (!imguiWantsMouse) {
    cameraController.update(dt, inputState);
}

// Your simulation and rendering
simulation.step(camera, settings);
renderer.draw();

// Render ImGui on top
ImGui::Render();
ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

window.swapBuffers();
```

---

## Part 6: Tips and Gotchas

### 1. ImGui Callbacks vs Your Callbacks

When you call `ImGui_ImplGlfw_InitForOpenGL(window, true)`, ImGui installs its own GLFW callbacks. The `true` parameter tells ImGui to chain to any existing callbacks you've set.

If you set callbacks AFTER ImGui init, you need to chain manually. Safest order:
1. Create window
2. Set your callbacks (or set them via Window class)
3. Initialize ImGui with `installCallbacks = true`

### 2. Style Customization

```cpp
ImGuiStyle& style = ImGui::GetStyle();
style.WindowRounding = 5.0f;
style.FrameRounding = 3.0f;
style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 0.9f);
```

### 3. Window Flags

```cpp
ImGuiWindowFlags flags = 0;
flags |= ImGuiWindowFlags_NoResize;
flags |= ImGuiWindowFlags_NoMove;
flags |= ImGuiWindowFlags_NoCollapse;
flags |= ImGuiWindowFlags_AlwaysAutoResize;

ImGui::Begin("Fixed Window", nullptr, flags);
```

### 4. Conditional Visibility

```cpp
static bool showSettings = true;

// Toggle with key (in your input handling)
if (keyPressed(GLFW_KEY_F1)) {
    showSettings = !showSettings;
}

// Only render if visible
if (showSettings) {
    ImGui::Begin("Settings");
    // ...
    ImGui::End();
}
```

### 5. Demo Window (Learning Resource)

ImGui includes a demo window that showcases all widgets:

```cpp
ImGui::ShowDemoWindow();  // Add this to see examples of everything
```

This is incredibly useful for learning — you can see widgets in action and the demo source code shows exactly how to use them.

---

## Summary Checklist

- [ ] Copy ImGui files to `external/imgui/`
- [ ] Update CMakeLists.txt with sources and include path
- [ ] Add initialization code (CreateContext, Init backends)
- [ ] Add per-frame code (NewFrame, your UI, Render)
- [ ] Add shutdown code (Shutdown backends, DestroyContext)
- [ ] Check `WantCaptureMouse` to gate camera input
- [ ] Create your settings panel using appropriate widgets
