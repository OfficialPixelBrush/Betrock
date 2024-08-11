#pragma once
#include <vector>

class nbtTag {
    public:
        uint8_t identifier = 0;
        std::string name = "";
};

class TAG_Byte : public nbtTag {
    public:
        int8_t data;
        TAG_Byte(std::string pName, int8_t pData) {
            this->identifier = 1;
            this->name = pName;
            this->data = pData;
        }
};

class TAG_Short : public nbtTag {
    public:
        int16_t data;
        TAG_Short(std::string pName, int16_t pData) {
            this->identifier = 2;
            this->name = pName;
            this->data = pData;
        }
};

class TAG_Int : public nbtTag {
    public:
        int32_t data;
        TAG_Int(std::string pName, int32_t pData) {
            this->identifier = 3;
            this->name = pName;
            this->data = pData;
        }
};

class TAG_Long : public nbtTag {
    public:
        int64_t data;
        TAG_Long(std::string pName, int64_t pData) {
            this->identifier = 4;
            this->name = pName;
            this->data = pData;
        }
};

class TAG_Float : public nbtTag {
    public:
        float data;
        TAG_Float(std::string pName, float pData) {
            this->identifier = 5;
            this->name = pName;
            this->data = pData;
        }
};

class TAG_Double : public nbtTag {
    public:
        double data;
        TAG_Double(std::string pName, double pData) {
            this->identifier = 6;
            this->name = pName;
            this->data = pData;
        }
};

class TAG_Byte_Array : public nbtTag {
    public:
        int32_t length;
        int8_t* data;
        TAG_Byte_Array(std::string pName, int32_t plength, int8_t* pData) {
            this->identifier = 7;
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
            this->identifier = 8;
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
            this->identifier = 9;
            this->name = pName;
            this->tagId = pTagId;
            this->length = pLength;
            this->data = pData;
        }
};

class TAG_Compound : public nbtTag {
    public:
        std::vector<nbtTag> data;
        TAG_Compound(std::string pName) {
            this->identifier = 10;
            this->name = pName;
        }

        int append(nbtTag tag) {
            data.push_back(tag);
            return 0;
        }
};

class TAG_Int_Array : public nbtTag {
    public:
        int32_t length;
        TAG_Int* data;
        TAG_Int_Array(std::string pName, TAG_Int* pData) {
            this->identifier = 11;
            this->name = pName;
            this->data = pData;
        }
};

class TAG_Long_Array : public nbtTag {
    public:
        int32_t length;
        TAG_Long* data;
        TAG_Long_Array(std::string pName, TAG_Long* pData) {
            this->identifier = 12;
            this->name = pName;
            this->data = pData;
        }
};