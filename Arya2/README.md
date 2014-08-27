## Building on Linux

Building on Linux requires the following libraries (latest releases should work):

- `OpenGL` (>= 3.0, with GLSL >= 4.0)
- `GLEW`
- `glm`
- `SDL2`

Additionally, the following tools are needed for building
- `gcc`
- `make`
- `cmake`

### Installation of packages on Arch based systems

For `gcc` and `make`, simply run
    `$ sudo pacman -S base-devel`

For the other packages:
    `$ sudo pacman -S cmake glew glm sdl2`

To build, simply navigate to the root directory of the repository and run

    $ cd build
    $ cmake .
    $ make

## Running on Linux

Put the required resources in `bin/`, these are currently in a private dropbox folder.
Then open `./bin/AryaGame` to run the game, and start a local game.

## Debugging
### Linux: simulating network delay for debugging synchronization issues

Find name of network device (usually eth0), on recent linux distributions: `$ ip link`

Add a delay: `$ tc qdisc add dev eth0 root netem delay 100ms`

Remove delay: `$ tc qdisc delete dev eht0 root`
