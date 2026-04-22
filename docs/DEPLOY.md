# Deployment Instructions

## Dependencies
Ensure you have the following installed on a Debian/Ubuntu-based system:
```bash
sudo apt-get install build-essential libsdl2-dev libsdl2-ttf-dev libjpeg-dev libpng-dev libcurl4-openssl-dev libvorbis-dev libphysfs-dev libfreetype6-dev gettext
```

## Compilation
1. Clean previous builds:
   ```bash
   make clean
   ```
2. Compile the binaries:
   ```bash
   make
   ```
   *Note: This will output `neverball` and `neverputt` in the root directory.*

## Running
Execute the compiled binaries directly from the root:
```bash
./neverball
# or
./neverputt
```

## Creating a Distribution Release
To package the game for distribution:
1. Ensure the `VERSION` file is updated.
2. Run the packaging script (if configured in `mk/` or `scripts/`). Typically, creating a tarball of the directory excluding `.git` is sufficient for source distribution.
3. For specific platforms (macOS, Windows), refer to `macosx/` or `emscripten/` directories for native build scripts.
