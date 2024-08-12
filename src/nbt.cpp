#include "nbt.h"
#include "helper.h"

std::string tagTypeName(int tag) {
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

int recursiveNbt(TAG_Compound* upperTag, uint8_t* data, size_t length, uint* index) {
    while (*index<length) {
        nbtTag* tag;
        uint8_t tagType = data[*index];
        *index+=1;
        // Handle TAG_End
        if (!tagType) {
            return 0;
        }
        // Get Name of Tag
        uint16_t nameLength = intReadArray(data,index,2);
        std::string tagName = "";
        for (uint16_t nL = 0; nL < nameLength; nL++) {
            tagName += data[*index+nL];
        }
        *index+=nameLength;
        std::cout << std::to_string(*index) << ":\t" << std::to_string(tagType) << "/"<<tagTypeName(tagType) << " - " << std::to_string(nameLength) << "->" << tagName << std::endl;
        
        // Create each kind of Tag
        switch (tagType) {
            case 1:
                tag = new TAG_Byte(tagName, data[*index]);
                break;
            case 2:
                tag = new TAG_Short(tagName, intReadArray(data,index,2));
                break;
            case 3:
                tag = new TAG_Int(tagName, intReadArray(data,index,4));
                break;
            case 4:
                tag = new TAG_Long(tagName, intReadArray(data,index,8));
                break;
            case 5:
                tag = new TAG_Float(tagName, float(intReadArray(data,index,4)));
                break;
            case 6:
                tag = new TAG_Double(tagName, double(intReadArray(data,index,8)));
            case 7: {
                int32_t size = intReadArray(data,index, 4);
                int8_t* arr;
                for (int32_t j = 0; j < size; j++) {
                    arr[j] = data[*index];
                    *index+=1;
                }
                tag = new TAG_Byte_Array(tagName, length, arr);
                break;
            }
            case 10: {
                tag = new TAG_Compound(tagName);
                TAG_Compound* compoundTag = dynamic_cast<TAG_Compound*>(tag);
                recursiveNbt(compoundTag, data, length, index);
                break;
            }
        }
        upperTag->append(tag);
    }
}

nbtTag nbt::loadNbt(uint8_t* data, size_t length) {
    TAG_Compound* rootTag = new TAG_Compound("");
    uint index = 3;
    recursiveNbt(rootTag, data,length,&index);
    if (!rootTag) {
        std::cerr << "Root tag assignment failed!" << std::endl;
    }
    return *rootTag;
}