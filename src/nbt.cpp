#include <iostream>
#include <math.h>
#include <cstdint>

enum nbtTagType {
    TAG_END,
    TAG_Byte,
    TAG_Short,
    TAG_Int,
    TAG_Long,
    TAG_Float,
    TAG_Double,
    TAG_Byte_Array,
    TAG_String,
    TAG_List,
    TAG_Compound,
    TAG_Int_Array,
    TAG_Long_Array
};

class nbtTag {
    public:
        uint8_t type = 0;
        std::string name = "";

        nbtTag(uint8_t pType, std::string pName) {
            type = pType;
            name = pName;
        }
};

std::string tagType(int tag) {
    switch(tag) {
        case 0:
            return "TAG_END";
        case 1:
            return "TAG_Byte";
        case 2:
            return "TAG_Short";
        case 3:
            return "TAG_Int";
        case 4:
            return "TAG_Long";
        case 5:
            return "TAG_Float";
        case 6:
            return "TAG_Double";
        case 7:
            return "TAG_Byte_Array";
        case 8:
            return "TAG_String";
        case 9:
            return "TAG_List";
        case 10:
            return "TAG_Compound";
        case 11:
            return "TAG_Int_Array";
        case 12:
            return "TAG_Long_Array";
    }
    return "Unknown";
}