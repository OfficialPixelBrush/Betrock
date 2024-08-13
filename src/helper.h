#pragma once
#include "global.h"
#include <fstream>
#include "compat.h"

uint64_t intReadFile(std::ifstream& f, uint size);
uint64_t intReadArray(uint8_t* arr, uint* index, uint size);