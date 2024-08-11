#include "nbt.h"
#include "helper.h"

nbtTag nbt::loadNbt(uint8_t* data, size_t length) {
    nbtTag* rootTag;
    uint numberOfTags = 0;
    for (uint i = 0; i < length; i++) {
        uint8_t tagType = data[i];
        i+=1;
        // Handle TAG_End
        if (!tagType) {
            continue;
        }
        // Get Name of Tag
        uint16_t nameLength = intReadArray(data,&i,2);
        std::string tagName = "";
        for (uint16_t nL = 0; nL < nameLength; nL++) {
            tagName += data[nL];
        }
        i+=nameLength;
        
        // Create each kind of Tag
        switch (tagType) {
        case 1:
            tag = new TAG_Byte(tagName, data[i]);
            break;
        case 2:
            tag = new TAG_Short(tagName, intReadArray(data,&i,2));
            break;
        case 3:
            tag = new TAG_Int(tagName, intReadArray(data,&i,4));
            break;
        case 4:
            tag = new TAG_Long(tagName, intReadArray(data,&i,8));
            break;
        case 5:
            tag = new TAG_Float(tagName, float(intReadArray(data,&i,4)));
            break;
        case 6:
            tag = new TAG_Double(tagName, double(intReadArray(data,&i,8)));
        case 7:
            //tag = new TAG_Byte_Array(tagName, )
            break;
        }
        std::cout << std::to_string(tagType) << ": \"" << tagName << "\"" << std::endl;
        break;
    }
    if (!tag) {
        std::cerr << "Invalid Tag!" << std::endl;
    }
    return *tag;
}

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