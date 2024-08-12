#pragma once
#include <iostream>
#include <math.h>
#include <cstdint>
#include "nbttag.h"
#include "helper.h"

class nbt {
    public:
        TAG_Compound loadNbt(uint8_t* data, size_t length);
};