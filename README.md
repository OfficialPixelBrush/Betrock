# Betrock
A C++ Application for exploring Minecraft Beta 1.7.3 Worlds,
written to look as close to the real deal as I can manage.

## Screenshot
![Betrock Screenshot](images/image.png)

## Build Instructions
Install the required programs and libraries.

## Setup
```bash
# Getting utilities
sudo apt install git cmake ninja-build g++
# Getting libraries
sudo apt install libgl1-mesa-dev libglfw3-dev libzip-dev libdeflate-dev libglm-dev libstb-dev libfmt-dev

# Setting up the repo
git clone --recurse-submodules https://github.com/OfficialPixelBrush/Betrock.git
cd Betrock
cmake -B build -S.
```

### Compilation
```bash
cmake --build build --config Debug --target all
```

### Running
```bash
cd build
./Betrock glacier
```
Tested on Linux Mint 22.1, Ubuntu 20.04 and WSL 2.4.10.0  (Debian 12)

### (Optional) Packing as tar.gz
```bash
cpack --config build/CPackConfig.cmake -G TGZ
```

### (Optional) Packing as AppImage
**Note**: The AppImage still requires all the folders inside `src/external/` else it will not run.
```bash
wget https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
chmod +x linuxdeploy-x86_64.AppImage
./linuxdeploy-x86_64.AppImage --appdir AppDir \
    --executable build/Betrock \
    --desktop-file Betrock.desktop \
    --icon-file icon.png \
    --output appimage
mv Betrock-x86_64.AppImage ./src/external/
cd ./src/external
zip -r Betrock_AppImage.zip *
rm *.AppImage
mv Betrock_AppImage.zip ../../
cd ../../
```

## Note for adding blocks
When exporting, ensure the exported block meshes are triangulated! Quad Meshes are not handled and **will** break rendering.

## Background
This project was an idea I had around the middle of 2023 but due to various factors, mainly me being busy with school and work, I was unable to realize it. As I'm unemployed right now, I figured I may as well get busy and learn some proper C++ and finally delve into OpenGL. This project is the result of that.

As a first step, I decided to see how far I could get with Python, to see how difficult it'd be to decypher Minecraft Beta's McRegion file format. Soon enough, I was able to extract the raw block data and dump it into a binary file. I reused the code of an old voxel world gen attempt I made in Godot, and reworked it to accept said binary files. Lo and behold, a chunk was loaded.

The next day I rewrote that code directly in GDScript to load whole McRegion files entirely in Godot, now allowing arbitrary regions to be loaded. However, due to each cube being rendered individually, the performance was rather abysmal. As a result, I figured I'd challenge myself and recreate something like it from scratch in C++ and OpenGL.

The [Python](https://github.com/OfficialPixelBrush/GodotBetaImport/blob/main/mcr.py) and [Godot](https://github.com/OfficialPixelBrush/GodotBetaImport/) experiments of this project are linked here.


## Code Style
### I'm new to C++!
This is moreso a disclaimer. I'm new to C++, so there'll be lots of C-isms as I try to get a proper grip on how C++ works and how to properly work with various OOP-isms. If there's code that looks out of place or outright bad, feel free to give tips as to how to improve it. I'd love to learn more!

### Never Nesting Rules
In an attempt to keep this Code somewhat readable for other people (and for me in the future), I've chosen to employ the **Never Nest** design pattern.

Please refer to the following video as to why: ["Why You Shouldn't Nest Your Code" by CodeAesthetic](https://www.youtube.com/watch?v=CFRhGnuXG-4)


## Progress
### Goals for Version 0.4.0
- [ ] Make fog act in world space, not view space!
- [ ] Biome Visuals
- [ ] Fix Chunk Loading Memory leak
- [ ] Windows Version
      - stb causing issues here

### Goals for Version 0.3.0
- [x] Smooth Lighting Fixes (turns out Minecrafts lighting just works like that)
- [x] Sky (was borked due to model loading being borked)

### Goals for Version 0.2.0
- Visual Flair
    - [x] Fog
    - [x] Smooth Lighting
    - [x] Ambient Occlusion
- Gameplay
    - [x] Chunk Loading
        - [x] Chunk Queue
        - [x] Fix Chunk Loading Crash
        - [ ] Fix Chunk Loading Memory leak
            - Caused by NBT Data
        - [x]  Threaded Chunk Loading
    - [x] Dynamic Chunk Loading as one moves
    - [x] Gravity
    - [x] Smooth Movement

### Goals for Version 0.1.0
- [x] Import Beta 1.7.3 compliant Minecraft worlds
    - [x] Decode Chunk Offset
    - [x] Decompress Chunk Data
        - [ ] Gzip
        - [x] Zlib
    - [x] Decode NBT Data
    - [x] Turn NBT Data into C++ Objects
    - [x] Extract Block Data
- [x] Render Block Data (via OpenGL)
    - [x] Render *something*
    - [x] Render a Texture
    - [x] Render a Cube
    - [x] Have a Camera that's keyboard and mouse controlled
    - [x] Add Lighting
    - [x] Model Importing
        - [x] OBJ
        - ~~[ ] glTF~~
    - [x] Render a Chunk
    - [x] Render a Region
    - [x] Render Multiple Regions
    - [x] Render per-face Block lighting
        - [x] Render Sky Lighting
        - [x] Render Block Lighting

## Resources
### Minecraft Wiki
- [Region file format](https://minecraft.wiki/w/Region_file_format)
- [NBT format](https://minecraft.wiki/w/NBT_format)
### wiki.vg (Now part of the Minecraft Wiki)
- [Protocol: Chunk Data and Update Light](https://minecraft.wiki/w/Minecraft_Wiki:Projects/wiki.vg_merge/Protocol#Chunk_Data_and_Update_Light)
### YouTube
- ["Why You Shouldn't Nest Your Code" by CodeAesthetic](https://www.youtube.com/watch?v=CFRhGnuXG-4)
- ["OpenGL Course - Create 3D and 2D Graphics With C++" by freeCodeCamp.org](https://youtu.be/45MIykWJ-C4)
### Other
- [Minecraft ID List](https://minecraft-ids.grahamedgecombe.com/)
