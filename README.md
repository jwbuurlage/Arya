## Building on Linux

Building on Linux requires the following libraries (latest releases should work):

- `OpenGL` (>= 3.0, with GLSL >= 4.0)
- `GLEW`
- `GLFW` (>= 3.0.0) 
- `glm`
- `SFML`
- `Poco`
- `lua 5.1` (arch package named lua51)
- `luabind` (compatible with lua 5.1, probably requires boost)

Also `CMake` and general development tools (such as gcc and make) are required for building

Simply navigate to the root directory of the repository and run

    $ cd build
    $ cmake .
    $ make

## Running on Linux

Put the required resources in `bin/`, these are currently in a private dropbox folder. Make a file config.txt in `bin/` and insert a line such as

    set sessionhash 10101 int

Where the number should be between 10101-10104

Then open `./bin/AryaGame` to run the game, and start a local game.
