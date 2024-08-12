#include "nbttag.h"

std::string nbtIdentifierName(int tag) {
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
            // Only exists in 12w07a/1.2.1+
            return "TAG_Int_Array";
        case 12:
            // Only exists in 1.12+
            return "TAG_Long_Array";
    }
    return "Unknown";
}