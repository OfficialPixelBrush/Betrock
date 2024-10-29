#pragma once
#include "global.h"
#include <fstream>
#include "compat.h"
#include <format>

uint64_t intReadFile(std::ifstream& f, uint size);
uint64_t intReadArray(uint8_t* arr, uint* index, uint size);
std::string printTripleFloat(float x, float y, float z);
std::string printTripleInt(int x, int y, int z);