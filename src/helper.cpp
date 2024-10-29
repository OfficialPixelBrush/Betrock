#include "helper.h"

// Read n-byte integer
uint64_t intReadFile(std::ifstream& f, uint size) {
	uint64_t integer = 0;
	for (uint i = 0; i < size; i++) {
		char byte = 0;
		f.read(&byte, 1);
		integer = integer << 8;
		integer |= (unsigned char)byte;
	}
	return integer;
}

uint64_t intReadArray(uint8_t* arr, uint* index, uint size) {
	uint64_t integer = 0;
	for (uint i = 0; i < size; i++) {
		char byte = arr[*index];
        *index+=1;
		integer = integer << 8;
		integer |= (unsigned char)byte;
	}
	return integer;
}

std::string printTripleFloat(float x, float y, float z) {
    return std::format("{}, {}, {}", x,y,z);
}

std::string printTripleInt(int x, int y, int z) {
    return std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z);
}