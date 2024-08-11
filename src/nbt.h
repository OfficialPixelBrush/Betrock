#pragma once
#include <iostream>
#include <math.h>
#include <cstdint>

class nbt {
    class nbtTag{};
    nbtTag loadNbt(uint8_t* data);
};