## Building on Linux

Building Borea on Linux requires the following libraries:

- `OpenGL` (>= 3.0, with GLSL >= 4.0)
- `GLEW`
- `GLFW` (>= 2.7.7) 
- `Poco` (>= 1.4.5)

Also `CMake` is required for building

Simply navigate to the map and run

    $ cd build
    $ cmake .
    $ make

Then open `./bin/AryaGame` to run the game.
