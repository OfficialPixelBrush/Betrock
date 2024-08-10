# Betrock
 C++ Application for exploring Minecraft Beta 1.7.3 Worlds

## Progress
- [ ] Import Beta 1.7.3 compliant Minecraft worlds
    - [x] Decode Chunk Offset
    - [x] Decompress Chunk Data
        - [ ] Gzip
        - [x] Zlib
    - [ ] Decode NBT Data
    - [ ] Extract Block Data
- [ ] Render Block Data (via OpenGL)
    - [ ] Render *something*

## Background
This project was an idea I had around the middle of 2023 but due to various factors, mainly me being busy with school and work, I was unable to realize it. As I'm unemployed right now, I figured I may as well get busy and learn some proper C++ and finally delve into OpenGL. This project is the result of that.

As a first step, I decided to see how far I could get with Python, to see how difficult it'd be to decypher Minecraft Beta's McRegion file format. Soon enough, I was able to extract the raw block data and dump it into a binary file. I reused the code of an old voxel world gen attempt I made in Godot, and reworked it to accept said binary files. Lo and behold, a chunk was loaded.

The next day I rewrote that code directly in GDScript to load whole McRegion files entirely in Godot, now allowing arbitrary regions to be loaded. However, due to each cube being rendered individually, the performance was rather abysmal. As a result, I figured I'd challenge myself and recreate something like it from scratch in C++ and OpenGL.

The [Python](https://github.com/OfficialPixelBrush/GodotBetaImport/blob/main/mcr.py) and [Godot](https://github.com/OfficialPixelBrush/GodotBetaImport/) experiments of this project are linked here.

## Resources
- [Region file format - Minecraft Wiki](https://minecraft.wiki/w/Region_file_format)
- [NBT format - Minecraft Wiki](https://minecraft.wiki/w/NBT_format)
- [OpenGL Course - Create 3D and 2D Graphics With C++](https://youtu.be/45MIykWJ-C4)