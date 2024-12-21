#pragma once
#include "../global.h"
#include <math.h>
#include "nbttag.h"
#include "../helper.h"
#include "../debug.h"
#include <stdexcept>

class nbt {
    public:
        TAG_Compound* loadNbt(uint8_t* data, size_t length);
        void freeNbt(TAG_Compound* rootTag);
};