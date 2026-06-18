# DEPLOY: Environment Setup

## Dependencies
* SDL2
* SDL2_ttf
* SDL2_mixer
* libjpeg
* libpng
* libogg
* libvorbis

## Build Instructions
1. Install dependencies via your package manager (e.g., `apt-get install libsdl2-dev libsdl2-ttf-dev libsdl2-mixer-dev libjpeg-dev libpng-dev`).
2. Run `make -j$(nproc)` in the repository root.
3. The executables `neverball` and `neverputt` will be built in the root directory.

## Map Compilation
If modifying level geometry (`.map` files):
1. Navigate to the map directory.
2. Run `../mapc <mapname>.map <mapname>.sol` to compile the BSP/solid geometry.

## Running
* Execute `./neverball` to launch the game.
* Add `--window` for windowed mode or `--debug` for verbose output.
