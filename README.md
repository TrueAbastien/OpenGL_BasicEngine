OpenGL Basic Engine
=======================

A simple C++20 CMake OpenGL engine, using an older version of OpenGL and its dependencies, with a strong emphasis on Physics with a basic interaction & response of colliding OBBs and Spheres.

This project was done in the context of an Engineering School project at ENSIMAG (Grenoble, France) in the topic of Digital Mechanics. As such, parts of the Engine were rushed and may result in unwanted behaviours.

Setup
===================

```
git clone --recursive https://github.com/TrueAbastien/OpenGL_BasicEngine.git
```

Submodules can also be initialized with:
```
git submodule init
git submodule update
```

Application should run on:
- Windows
- Linux
- Mac

Simply run the `CMakeLists.txt` as such:
```
mkdir build
cd build
cmake ..
```

Usage
===================

The current main application can be controlled by using a mouse and keyboard as such:
- **ZQSD/WASD**: move the Camera center around depending on the screen orientation
- **Mouse RB**: hold and slide to rotate the Camera
- **Mouse Scroll**: adjust the distance from the Camera center

An easy-to-use ImGui debug window should also help you pick and control the scene you want to show.

Sources
===================

Most of the used code was written for the project.
Altough, most of the algorithms followed the context of research papers on various topics.

Here is the list of all used sources:

