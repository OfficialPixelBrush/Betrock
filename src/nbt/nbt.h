#pragma once
#include "../global.h"
#include <math.h>
#include "nbttag.h"
#include "../helper.h"

class nbt {
    public:
        TAG_Compound loadNbt(uint8_t* data, size_t length);
};