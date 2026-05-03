# 3D City Simulator

A lightweight, real-time 3D city environment built with **C++**, **OpenGL 3.3**, and **GLFW**. This simulator features a procedural environment with dynamic day/night cycles, moving traffic, animated wildlife, and custom lighting shaders.

## 🚀 Features
- **Real-Time 3D Rendering**: High-performance rendering using modern OpenGL (GLSL 3.3).
- **Day/Night Toggle**: Smooth transition between a bright daytime environment and a neon-lit nighttime city (Press `SPACE`).
- **Dynamic Traffic**: Procedurally animated vehicles (cars, trucks, and buses) with rotating wheels and functional headlights at night.
- **Environment Lighting**: 
  - Directional lighting for the Sun and Moon.
  - Point-light simulation for city lamps and vehicle glow.
  - Material-based emissive shaders for windows and traffic lights.
- **Interactive Camera**: FPS-style movement (WASD + Mouse) to explore the city.
- **Animated Wildlife**: Procedural birds with flapping wing animations.

## 🛠 Prerequisites
To build and run this project, you will need:
- **C++ Compiler**: Support for C++11 or higher.
- **GLAD**: Multi-Language GL/GLES/WGL/EGL/GLX Loader-Generator.
- **GLFW**: For window management and input handling.
- **GLM**: OpenGL Mathematics library for matrix and vector operations.

## 🕹 Controls
| Key | Action |
| :--- | :--- |
| **W / S** | Move Forward / Backward |
| **A / D** | Strafe Left / Right |
| **Mouse** | Look around (Rotate Camera) |
| **SPACE** | Toggle Day/Night Mode |
| **ESC** | Exit Simulator |

## 🏗 Project Structure
- `Vertex Shader`: Handles coordinate transformations, normal calculations, and passes fragment positions.
- `Fragment Shader`: Calculates the Blinn-Phong lighting model, ambient city glow, and manages emissive materials.
- `Camera Class`: Manages view matrices and user input processing.
- `Procedural Functions`: Dedicated functions for rendering houses, trees, traffic lights, and lamp posts.

## 📸 Technical Overview
The simulator uses a custom **drawShape** abstraction that allows for easy placement of cubes and spheres with specific scaling, rotation, and color properties. All objects are rendered using Vertex Array Objects (VAOs) and Vertex Buffer Objects (VBOs) for optimal GPU performance.
