#include "nbt.h"

std::string printDepth(uint depth) {
    std::string result = "";
    for (uint i = 0; i < depth; i++) {
        result += "\t";
    }
    return result;
}
int recursiveNbt(nbtTag* upperTag, uint8_t* data, size_t length, uint* index, uint depth = 0, uint8_t defaultTagType = 0) {
    while (*index < length) {
        std::unique_ptr<nbtTag> tag; // Use a unique_ptr for automatic memory management
        uint8_t tagType;
        std::string tagName = "";

        // Read the tag type and name
        if (!defaultTagType) {
            tagType = data[*index];
            *index += 1;

            // Handle TAG_End
            if (!tagType) {
                return 0;
            }

            // Get Name of Tag
            uint16_t nameLength = intReadArray(data, index, 2);
            for (uint16_t nL = 0; nL < nameLength; nL++) {
                tagName += data[*index + nL];
            }
            *index += nameLength;
            if (nbtDebug) {
                std::cout << printDepth(depth) <<"┠" << nbtIdentifierName(tagType) << ": " << tagName << std::endl;
                //std::cout << std::to_string(*index) << ":\t" << std::to_string(tagType) << "/"<<nbtIdentifierName(tagType) << " - " << std::to_string(nameLength) << "->" << tagName << std::endl;
            }
        }

        // Create each kind of Tag
        switch (tagType) {
            case 1:
                tag = std::make_unique<TAG_Byte>(tagName, intReadArray(data, index, 1));
                break;
            case 2:
                tag = std::make_unique<TAG_Short>(tagName, intReadArray(data, index, 2));
                break;
            case 3:
                tag = std::make_unique<TAG_Int>(tagName, intReadArray(data, index, 4));
                break;
            case 4:
                tag = std::make_unique<TAG_Long>(tagName, intReadArray(data, index, 8));
                break;
            case 5:
                tag = std::make_unique<TAG_Float>(tagName, float(intReadArray(data, index, 4)));
                break;
            case 6:
                tag = std::make_unique<TAG_Double>(tagName, double(intReadArray(data, index, 8)));
                break;
            case 7: {
                int32_t size = intReadArray(data, index, 4);
                auto arr = std::make_unique<int8_t[]>(size);
                for (int32_t j = 0; j < size; j++) {
                    arr[j] = data[*index + j];
                }
                *index += size;
                tag = std::make_unique<TAG_Byte_Array>(tagName, size, std::move(arr));
                break;
            }
            case 8: {
                uint16_t size = intReadArray(data, index, 2);
                std::string tagString = ""; // This should be populated as needed
                tag = std::make_unique<TAG_String>(tagName, size, tagString);
                break;
            }
            case 9: {
                uint8_t underlyingTagType = intReadArray(data, index, 1);
                uint16_t size = intReadArray(data, index, 2);
                tag = std::make_unique<TAG_List>(tagName, underlyingTagType, size);
                TAG_List* listTag = dynamic_cast<TAG_List*>(tag.get());
                recursiveNbt(listTag, data, *index + size, index, depth + 1, underlyingTagType);
                break;
            }
            case 10: {
                tag = std::make_unique<TAG_Compound>(tagName);
                TAG_Compound* compoundTag = dynamic_cast<TAG_Compound*>(tag.get());
                recursiveNbt(compoundTag, data, length, index, depth + 1);
                break;
            }
            default:
                std::cerr << "Unknown or Unimplemented Tag!" << std::endl;
                return 1;
        }

        // Append the tag to upperTag (assuming it handles ownership correctly)
        if (auto* compoundTag = dynamic_cast<TAG_Compound*>(upperTag)) {
            compoundTag->append(std::move(tag)); // Move the unique_ptr
        } else if (auto* listTag = dynamic_cast<TAG_List*>(upperTag)) {
            listTag->append(std::move(tag)); // Move the unique_ptr
        } else {
            std::cerr << "Error: upperTag is neither a TAG_Compound nor a TAG_List!" << std::endl;
            return 1;
        }
    }
    return 0;
}

TAG_Compound* nbt::loadNbt(uint8_t* data, size_t length) {
    TAG_Compound* rootTag = new TAG_Compound("");
    uint index = 3;
    recursiveNbt(rootTag, data,length,&index);
    return rootTag;
}

void cleanupNbtTag(nbtTag* tag) {
    if (!tag) return;

    // Handle compound tags
    if (auto* compoundTag = dynamic_cast<TAG_Compound*>(tag)) {
        // Clean up all contained tags
        for (auto& childTag : compoundTag->data) {
            cleanupNbtTag(childTag.get());
        }
    }
    // Handle list tags
    else if (auto* listTag = dynamic_cast<TAG_List*>(tag)) {
        // Clean up all contained tags
        for (auto& childTag : listTag->data) {
            cleanupNbtTag(childTag.get());
        }
    }
    // Other tag types don't need special cleanup as they don't own other tags
    
    // Note: We don't delete the tag here because the unique_ptr will handle that
    // when it goes out of scope
}

void nbt::freeNbt(TAG_Compound* rootTag) {
    if (!rootTag) return;
    
    // Clean up all tags recursively
    cleanupNbtTag(rootTag);
    
    // Delete the root tag itself
    delete rootTag;
}