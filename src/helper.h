#pragma once
#include "global.h"
#include <fstream>

uint64_t intReadFile(std::ifstream& f, uint size);
uint64_t intReadArray(uint8_t* arr, uint* index, uint size);