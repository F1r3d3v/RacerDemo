# GK1-Racer

**Overview**

GK1-Racer is a racing game demonstration built upon the GK1-Engine, a custom 3D game engine leveraging modern OpenGL.  This project showcases the engine's capabilities, including rendering, physics, input handling, and scene management.

---

## Project Structure

The project is organized into two main directories: the engine itself and the game built upon it.

### GK1-Engine/

This directory contains the core game engine, designed to be reusable and extensible.

-   **include/Engine/**:  Header files defining the engine's API. These are organized into logical subdirectories:
    -   **Loader/**:  Classes for loading resources (models, materials).  Includes support for OBJ and MTL formats.
    -   **Objects/**:  Classes representing game objects like meshes, models, lights (point and spot), cameras, scene nodes, skyboxes, and terrain.
    -   **Resource/**: Classes for managing resources such as materials, shaders, and textures.
    -   **.h files**: Core engine components including application setup (App.h), input handling (Input.h), rendering (Renderer.h), resource management (ResourceManager.h), scene management (Scene.h), transformations (Transform.h), and utility classes (Log.h).
-   **src/**: Source files implementing the engine's functionality.
-   **lib/**:  Third-party libraries used by the engine.

### GK1-Racer/

This directory contains the racing game implementation built using the GK1-Engine.

-   **assets/**: Game-specific assets, such as models and textures.
-   **include/**:  Header files specific to the racing game.  Organized into subdirectories:
    -   **CameraController/**:  Different camera controller implementations (Base, Fly, Racing).
    -   **Objects/**: Game-specific objects (e.g., Vehicle).
    -   **Physics/**:  Physics-related components (PhysicsManager, VehicleController).
    -   **.h files**:  Main application class (MyApp.h) and configuration settings (Config.h).
-   **src/**: Source files for the game logic.
- **lib/**: Third-party libraries for the game.

---

## Features

GK1-Racer demonstrates a range of features provided by the GK1-Engine:

-   **Modern OpenGL Rendering:**  Uses a programmable rendering pipeline with custom shaders.  Supports features like:
    -   Phong shading with normal interpolation.
    -   Texturing (diffuse, specular, normal, height maps).
    -   Tessellation shaders for terrain height variations.
    -   Fog effects.
    -   Skybox rendering (with day/night blending).
    -   Transparency and blending.
    -   Depth buffering.
    -   Uniform Buffer Objects (UBOs) for efficient data transfer to shaders.
    -   Support for both perspective and orthographic projections.

-   **Dynamic Scene Management:**
    -   Hierarchical scene graph using `SceneNode` objects.
    -   Flexible transformation system (position, rotation, scale) for all game objects.
    -   Support for multiple lights (point lights and spotlights) with attenuation and color control.  A `LightManager` handles multiple lights efficiently.
    -   A `Scene` class encapsulates the rendering process and manages the scene hierarchy, camera, skybox, and lighting.

-   **Physics Integration:**
    -   Uses the Bullet Physics library for realistic vehicle dynamics and collision detection.
    -   A `PhysicsManager` class wraps the Bullet dynamics world.
    -   `VehicleController` class handles vehicle movement, steering, and braking.

-   **Camera Control:**
    -   Multiple camera modes, switchable during gameplay:
        -   A static overview camera.
        -   A static camera that tracks the moving object.
        -   A "racing" camera that follows the vehicle with configurable stiffness, offset, and velocity-based adjustments.
        -   A free-fly camera for unrestricted movement.
    -   `BaseCameraController` provides a base class for camera control, handling mouse input.

-   **Resource Management:**
    -   A `ResourceManager` class (singleton) handles loading and managing various resources (models, materials, textures, shaders).
    -   Custom loaders for OBJ models and MTL materials, supporting:
        -   Vertex positions, normals, texture coordinates, tangents, and bitangents.
        -   Multiple meshes per OBJ file.
        -   Material properties (ambient, diffuse, specular colors, shininess).
        -   Texture maps (ambient, diffuse, specular, normal, height).
    -   `LoaderFactory` and `LoaderManager` for easy extension with new resource types.

-   **User Interface:**
    -   Uses Dear ImGui for an in-game user interface (control panel).
    -   GLFW is used for window creation, input handling, and OpenGL context management.

-   **Input Handling:**
      - The custom `Input` class offers a layer of abstraction for handling keyboard and mouse input, including:
          - Key press/release/down/up states.
          - Mouse button press/release/down/up states.
          - Mouse position and delta.
          - Mouse scroll delta.

---

## Dependencies

The project relies on the following external libraries:

-   [GLFW](https://www.glfw.org/): Window creation, input handling, and OpenGL context management.
-   [GLM](https://github.com/g-truc/glm):  Header-only mathematics library for 3D graphics.
-   [Dear ImGui](https://github.com/ocornut/imgui):  Bloat-free Immediate Mode Graphical User interface.
-   [Bullet Physics](https://github.com/bulletphysics/bullet3):  Physics simulation library.
-   [stb](https://github.com/nothings/stb): Single-file libraries for image loading (primarily `stb_image.h`).
- [Glad](https://glad.dav1d.de/): A multi-language GL/GLES/EGL/GLX/WGL Loader-Generator.

---

## Getting Started & Building the Project

1.  **Clone the Repository:**

    ```bash
    git clone https://github.com/F1r3d3v/GK1-Racer.git
    ```

2.  **Open the Project:**

    Open the solution file `GK1-Racer.sln` in Visual Studio.

3.  **Configure Build:**

    Select the desired build configuration (e.g., Debug or Release).

4.  **Build the Solution:**

    Build the solution (e.g., by pressing Ctrl+Shift+B or selecting Build > Build Solution).

5.  **Run the Executable:**

    After a successful build, run the executable generated in the output directory.