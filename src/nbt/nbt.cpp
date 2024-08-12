#include "nbt.h"

std::string printDepth(uint depth) {
    std::string result = "";
    for (uint i = 0; i < depth; i++) {
        result += "\t";
    }
    return result;
}

int recursiveNbt(TAG_Compound* upperTag, uint8_t* data, size_t length, uint* index, uint depth = 0, uint8_t defaultTagType = 0) {
    while (*index<length) {
        nbtTag* tag;
        uint8_t tagType;
        std::string tagName = "";
        // If no default paramaters have been provided, read them from the array
        if (!defaultTagType) {
            tagType = data[*index];
            *index+=1;
            // Handle TAG_End
            if (!tagType) {
                return 0;
            }
            // Get Name of Tag
            uint16_t nameLength = intReadArray(data,index,2);
            for (uint16_t nL = 0; nL < nameLength; nL++) {
                tagName += data[*index+nL];
            }
            *index+=nameLength;
        }
        //std::cout << printDepth(depth) <<"┠" << nbtIdentifierName(tagType) << ": " << tagName << std::endl;
        
        // Create each kind of Tag
        switch (tagType) {
            case 1:
                tag = new TAG_Byte(tagName, intReadArray(data,index,1));
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
                break;
            case 7: {
                int32_t size = intReadArray(data,index, 4);
                int8_t* arr = new int8_t[16*128*16];
                //std::cout << "\tSize: " << std::to_string(size) << std::endl;
                for (int32_t j = 0; j < size; j++) {
                    arr[j] = data[*index+j];
                }
                *index+=size;
                tag = new TAG_Byte_Array(tagName, length, arr);
                break;
            }
            case 8: {
                uint16_t size = intReadArray(data,index, 2);
                std::string tagString = "";
                tag = new TAG_String(tagName, size, tagString);
                break;
            }
            // TODO: Implement TAG_List
            /*case 9: {
                uint16_t size = intReadArray(data,index, 2);
                tag = new TAG_List(tagName);
                TAG_List* listTag = dynamic_cast<TAG_List*>(tag);
                recursiveNbt(listTag, data, length, index, tagType, tagName);
                break;
            }*/
            case 10: {
                tag = new TAG_Compound(tagName);
                TAG_Compound* compoundTag = dynamic_cast<TAG_Compound*>(tag);
                recursiveNbt(compoundTag, data, length, index, depth+1);
                break;
            }
            default:
                std::cerr << printDepth(depth) << "Unknown or Unimplemented Tag " << nbtIdentifierName(tagType) << "!" << std::endl;
                return 1;
        }
        upperTag->append(std::unique_ptr<nbtTag>(tag));
    }
    return 0;
}

TAG_Compound nbt::loadNbt(uint8_t* data, size_t length) {
    TAG_Compound* rootTag = new TAG_Compound("");
    uint index = 3;
    recursiveNbt(rootTag, data,length,&index);
    if (!rootTag) {
        std::cerr << "Root tag assignment failed!" << std::endl;
    }
    return *rootTag;
}