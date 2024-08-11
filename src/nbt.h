#pragma once
#include <iostream>
#include <math.h>
#include <cstdint>
#include "nbttag.h"

class nbt {
    public:
        nbtTag loadNbt(uint8_t* data, size_t length);
};