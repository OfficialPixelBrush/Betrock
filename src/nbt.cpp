#include <iostream>
using namespace std;

/*
class NbtTag {
    public:
        int tagId;
        string tagName;
        string name;
        NbtTag(string pName) {
            name = pName;
        }
};

class NbtTag_TAG_END: public NbtTag {
    NbtTag_TAG_END():NbtTag("") {
        tagId = 0;
        tagName = "TAG_END";
    }
};

class NbtTag_TAG_Byte: public NbtTag {
    NbtTag_TAG_Byte(string pName):NbtTag(pName) {
        tagId = 1;
        tagName = "TAG_Byte";
    }
};

int main() {
    NbtTag_TAG_Byte("Test");
}
*/

string tagType(int tag) {
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