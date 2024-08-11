#include "nbt.h"
using namespace std;

class nbtTag {
    public:
        std::string name = "";
};

class TAG_Byte : public nbtTag {
    public:
        int8_t data;
        TAG_Byte(std::string pName, int8_t pData) {
            this->name = pName;
            this->data = pData;
        }
};

class TAG_Short : public nbtTag {
    public:
        int16_t data;
        TAG_Short(std::string pName, int16_t pData) {
            this->name = pName;
            this->data = pData;
        }
};

class TAG_Int : public nbtTag {
    public:
        int32_t data;
        TAG_Int(std::string pName, int32_t pData) {
            this->name = pName;
            this->data = pData;
        }
};

class TAG_Long : public nbtTag {
    public:
        int64_t data;
        TAG_Long(std::string pName, int64_t pData) {
            this->name = pName;
            this->data = pData;
        }
};

class TAG_Float : public nbtTag {
    public:
        float data;
        TAG_Float(std::string pName, float pData) {
            this->name = pName;
            this->data = pData;
        }
};

class TAG_Double : public nbtTag {
    public:
        double data;
        TAG_Double(std::string pName, double pData) {
            this->name = pName;
            this->data = pData;
        }
};

class TAG_Byte_Array : public nbtTag {
    public:
        int32_t length;
        int8_t* data;
        TAG_Byte_Array(std::string pName, int32_t plength, int8_t* pData) {
            this->name = pName;
            this->length = plength;
            this->data = pData;
        }
};

class TAG_String : public nbtTag {
    public:
        uint16_t length;
        int8_t* data;
        TAG_String(std::string pName, uint16_t pLength, int8_t* pData) {
            this->name = pName;
            this->length = pLength;
            this->data = pData;
        }
};

class TAG_List : public nbtTag {
    public:
        int8_t tagId;
        int32_t length;
        nbtTag* data;
        TAG_List(std::string pName, int8_t pTagId, uint16_t pLength, nbtTag* pData) {
            this->name = pName;
            this->tagId = pTagId;
            this->length = pLength;
            this->data = pData;
        }
};

class TAG_Compound : public nbtTag {
    public:
        nbtTag* data;
        TAG_Compound(std::string pName, nbtTag* pData) {
            this->name = pName;
            this->data = pData;
        }
};

class TAG_Int_Array : public nbtTag {
    public:
        int32_t length;
        TAG_Int* data;
        TAG_Int_Array(std::string pName, TAG_Int* pData) {
            this->name = pName;
            this->data = pData;
        }
};

class TAG_Long_Array : public nbtTag {
    public:
        int32_t length;
        TAG_Long* data;
        TAG_Long_Array(std::string pName, TAG_Long* pData) {
            this->name = pName;
            this->data = pData;
        }
};

nbtTag loadNbt(uint8_t* data) {
    for (auto d : data) {
        printf("%c", d);
    }
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