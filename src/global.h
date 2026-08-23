// Used for libs that're needed everywhere
#pragma once
#include <iostream>
#include <cstdint>
#include <cerrno>
#include "compat.h"
// ---------------------------------------------------------------------------
// Global base-data-path used to resolve shaders, textures, models, saves.
// Set once in main() before any other subsystem initialises.
// ---------------------------------------------------------------------------
#include <string>
namespace LegacyWorldViewer {
    inline std::string& basePath() {
        static std::string path;
        return path;
    }
}
