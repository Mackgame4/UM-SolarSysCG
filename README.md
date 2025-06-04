#  ☀️ Trivial-Solar-System-CG

## About the Project


This project was developed for the subject of Computer Graphics from University of Minho - Software Engineering degree.

#### Grade ⭐️ ⚠️ Change me ⚠️/20

### Demo 📽️ ⚠️ Change me ⚠️

![Demo Image](./readme/deo.png)

⚠️ Change me ⚠️ link to video

### Features

* ⚠️ Change me ⚠️ 

### The goal ⛳️

This project aims to develop a graphical simulation of the Solar System. The project is divided into four phases, each adding more complexity and features to the simulation. The final goal is to create an interactive and visually appealing representation of the Solar System using OpenGL and GLUT.

### About the Code 🧑‍💻

* ⚠️ Change me ⚠️ 

## Getting Started 🚀

#### Cloning the repository

```bash
$ git clone https://github.com/afooonso/Trivial-Solar-System-CG.git
$ git clone git@github.com:afooonso/Trivial-Solar-System-CG.git
$ cd Trivial-Solar-System-CG
```

#### Running the project
##### Prerequisites:
- [vcpkg](https://vcpkg.io/en/getting-started)
- [CMake](https://cmake.org/download/)
- [MSBuild](https://visualstudio.microsoft.com/downloads/) on Windows or GCC/CLang on Linux and macOS




#### Setting up the Build Environment
Inside the project folder, you can run the following commands to set up the project:
#### Windows/Linux
First, configure the project with `CMake` and specify the `vcpkg` toolchain file. This ensures that all dependencies are properly linked via `vcpkg`.

```bash
$ cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=[path to vcpkg]/scripts/buildsystems/vcpkg.cmake
cmake --build build
```
This will generate the executables for both the engine and generator inside the build directory.

### Using the Generator
``` 
.\build\generator\Debug\generator.exe  <figure>  <inputs> <outputfile>
```
### Generate a figure

**List of figures and inputs:**:
1. `./generator plane <length> <divisions> <output>`
2. `./generator box <length> <divisions> <output>`
3. `./generator cone <radius> <height> <slices> <stacks> <output>`
4. `./generator sphere <radius> <slices> <stacks> <output file>`
5. `./generator cylinder <radius> <height> <slices> <output>`
6. `./generator donut <outerRadius> <innerRadius> <slices> <stacks> <output file> `
7. `./generator patch <input_patch_file> <tesselation> <output file>`



### Using the engine
The output file can be a `.3d` file, `.obj`, or an `.xml` scene.  
```
.\build\engine\Debug\engine.exe  <outputfile>
```



## Developed by 🧑‍💻:

- [Afonso Pedreira](https://github.com/afooonso)
- [Fábio Magalhães](https://github.com/Mackgame4)

