#include <iostream>
#include <fstream>
using namespace std;

uint intread(ifstream & file, uint size);
string csDecode(uint cs);
class regionDecoder {
    private:
    int decodeRegion(ifstream& f);
    public:
    int getRegion(int regionX, int regionZ);
};