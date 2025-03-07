# GPU Butterflies

A little OpenGL/WebGL demo made with raylib.

Animated and tweak-able flock of butterflies - all computations done on GPU with minimal CPU involvement.
Hundreds of thousands of butterflies could be rendered simultaneously.
Could be used as a light GPU stress-test.

![screenshot 1](images/screenshot01.png)
![screenshot 2](images/screenshot02.png)

## Building on Linux

### Prerequisites
Clone [raylib](https://github.com/raysan5/raylib) repository into any suitable directory and build and install it according to the [instructions](https://github.com/raysan5/raylib/wiki/Working-on-GNU-Linux).

### Build
Build with the makefile:
```
    make clean
    make RAYLIB_PATH=[raylib source directory]
```

### HTML5/WASM

### Prerequisites
Install [Emscripten](https://emscripten.org/) according to the [instructions](https://emscripten.org/docs/getting_started/downloads.html)

### Build raylib WASM library

Navigate to raylib source directory

```
    cd src
    make clean
    make PLATFORM=PLATFORM_WEB GRAPHICS=GRAPHICS_API_OPENGL_ES3
    mv libraylib.a libraylib.web.a
```
### Build web-page

Navigate back to this repo's directory and build with the makefile:

```
    make -f wasm.mk RAYLIB_PATH=[raylib source directory]
```

The output web-page files will be stored in the build_web directory.
