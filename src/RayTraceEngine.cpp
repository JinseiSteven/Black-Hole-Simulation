//
// Created by StephanVisser on 10/26/2025.
//

#include <GL/glew.h>
#include <iostream>
#include <cmath>

#include <RayTraceEngine.h>
#include <Config.h>
#include <Camera.h>
#include <InputHandler.h>
#include <Simulation.h>
#include <Renderer.h>
#include <Settings.h>
#include <Utils.h>


RayTraceEngine::RayTraceEngine() : RayTraceEngine(BASE_SCREEN_WIDTH, BASE_SCREEN_HEIGHT) {}

RayTraceEngine::RayTraceEngine(const int width, const int height) :
    screen_width(width),
    screen_height(height)
{
    GL_init();

    settings = std::make_shared<Settings>();

    if (!check_render_scale(Config::RENDER_SCALE)) {
        throw std::runtime_error("Render scale not supported. The render texture needs to be divisible by the GPU workgroups");
    }

    // setting up the virtual camera for the scene
    camera = std::make_unique<Camera>(
        Config::FOV_Y,
        static_cast<float>(screen_width) / static_cast<float>(screen_height),
        Config::Z_NEAR,
        Config::Z_FAR,
        settings
        );

    // inputhandler class dependant on the camera class, responsible for movement/command presses
    input_handler = std::make_unique<InputHandler>(window, camera.get());
    glfwSetMouseButtonCallback(window, InputHandler::MouseButtonCallbackWrapper);
    glfwSetCursorPosCallback(window, InputHandler::CursorPosCallbackWrapper);
    glfwSetKeyCallback(window, InputHandler::KeyCallbackWrapper);

    // setting up both the renderer and simulation
    // (renderer first, since the simulation needs the correct output texture id)
    renderer = std::make_unique<Renderer>(
        camera.get(),
        static_cast<unsigned int>(static_cast<float>(width) * settings->GetRenderScale()),
        static_cast<unsigned int>(static_cast<float>(height) * settings->GetRenderScale()),
        Config::SCREENQUAD_VERT.c_str(),
        Config::SCREENQUAD_FRAG.c_str(),
        Config::RADIALMESH_VERT.c_str(),
        Config::RADIALMESH_FRAG.c_str()
        );
    simulation = std::make_unique<Simulation>(
        settings,
        static_cast<unsigned int>(static_cast<float>(width) * settings->GetRenderScale()),
        static_cast<unsigned int>(static_cast<float>(height) * settings->GetRenderScale()),
        renderer->GetScreenTextureID(),
        Config::COMPUTE_SIM.c_str()
        );
    
    simulation->UpdatePlanetsData(Utils::LoadPlanetsFromFile(Config::PLANETS_TXT_FILE));

    rebuildRadialMesh();
}

void RayTraceEngine::GL_init() {
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    // setting up the context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(
        static_cast<int>(screen_width),
        static_cast<int>(screen_height),
        Config::TITLE.c_str(),
        nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window\n");
    }
    glfwMakeContextCurrent(window);

    // this so we can use modern GL functions, not just old ones
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        glfwDestroyWindow(window);
        glfwTerminate();
        throw std::runtime_error("Failed to initialize GLEW\n");
    }

    // now we also store a pointer to the current raytrace engine inside the window, so we can fetch it and its
    // components from any GLFW callback
    glfwSetWindowUserPointer(window, this);

    // actually making the viewport now
    glViewport(0, 0, static_cast<int>(screen_width), static_cast<int>(screen_height));
}

void RayTraceEngine::rebuildRadialMesh() const {
    const int num_rings = settings->GetRadialMeshRings();
    const float inner_radius = settings->GetRadialMeshInnerRadius();
    const float outer_radius = settings->GetRadialMeshOuterRadius();

    simulation->UpdateRadialHeightMap(
        num_rings,
        inner_radius,
        outer_radius);
    renderer->RebuildRadialMesh(
        num_rings,
        settings->GetRadialMeshSpokes(),
        inner_radius,
        outer_radius);

    renderer->UpdateRadialHeightmap(simulation->GetRadialHeightMap());
}


bool RayTraceEngine::check_render_scale(const float render_scale) const{
    // need to round the pixels up
    int trace_width = static_cast<int>(std::round(static_cast<float>(screen_width) * render_scale));
    int trace_height = static_cast<int>(std::round(static_cast<float>(screen_height) * render_scale));

    // we check whether we can divide the pixels up nicely among the workgroups
    bool width_is_divisible = (trace_width % Config::WORKGROUP_SIZE_X) == 0;
    bool height_is_divisible = (trace_height % Config::WORKGROUP_SIZE_Y) == 0;

    return width_is_divisible && height_is_divisible;
}

RayTraceEngine::~RayTraceEngine() {
    if (window) glfwDestroyWindow(window);
    glfwTerminate();
}

void RayTraceEngine::step() {
    // a single step of the simulation cycle, from input handling to rendering
    glfwPollEvents();

    if (settings->ConsumeRenderChanges()) {
        rebuildRadialMesh();
    }

    // TODO handling input is done via callbacks, we could do some force based movement though

    // simulating
    simulation->step(camera.get());

    // rendering
    renderer->draw();

    glfwSwapBuffers(window);
}

void RayTraceEngine::tick(double deltaTime) {
    // we'll do this once the simulation is actually able to run in near-realtime
    // we can then also use some kind of timescale variable in the config to alter the speed of the simulation
}

