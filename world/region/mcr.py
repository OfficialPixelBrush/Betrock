import math
import zlib
import gzip
from io import BytesIO

region_x = 0
region_z = 0
kib = 1024
header_index = 0
regionFile = f"r.{region_x}.{region_z}.mcr"
f = open(regionFile, "rb")

print(f"X: {region_x}\nZ: {region_z}")

def intread(file, length):
    return int.from_bytes(file.read(length),byteorder='big')

def floatread(file, length):
    return float.from_bytes(file.read(length),byteorder='big')

def tagType(tag):
    if tag==0: # TAG_END
        return "TAG_END"
    elif tag==1: # TAG_Byte
        return "TAG_Byte"
    elif tag==2: # TAG_Short
        return "TAG_Short"
    elif tag==3: # TAG_Int
        return "TAG_Int"
    elif tag==4: # TAG_Long
        return "TAG_Long"
    elif tag==5: # TAG_Float
        return "TAG_Float"
    elif tag==6: # TAG_Double
        return "TAG_Double"
    elif tag==7: # TAG_Byte_Array
        return "TAG_Byte_Array"
    elif tag==8: # TAG_String
        return "TAG_String"
    elif tag==9: # TAG_List
        return "TAG_List"
    elif tag==10: # TAG_Compound
        return "TAG_Compound"
    elif tag==11: # TAG_Int_Array
        return "TAG_Int_Array"
    elif tag==12: # TAG_Long_Array
        return "TAG_Long_Array"
    else:
        return "Unknown"

def decodeTag(file):
    tag = intread(file,1)
    tagNameLength = intread(file,2)
    tagName = ""
    data = None
    if tagNameLength > 0:
        tagName = file.read(tagNameLength).decode("utf-8")
    if tag==0: # TAG_END
        data = None
    elif tag==1: # TAG_Byte
        data = intread(file,1)
    elif tag==2: # TAG_Short
        data = intread(file,2)
    elif tag==3: # TAG_Int
        data = intread(file,4)
    elif tag==4: # TAG_Long
        data = intread(file,8)
    elif tag==5: # TAG_Float
        data = floatread(file,4)
    elif tag==6: # TAG_Double
        data = floatread(file,8)
    elif tag==7: # TAG_Byte_Array
        size = intread(file,4)
        data = []
        for i in range(size):
           data.append(intread(file,1))
        #data = f"{len(data)} Entries"
        x = region_x + (header_index % 32 * 16)
        z = region_z + (math.floor(header_index / 32) * 16)
        open(f'./extracted/chunk_{x}_{z}.bin', 'wb').write(bytes(data))
    elif tag==8: # TAG_String
        length = intread(file,2)
        data = file.read(length).decode("utf-8")
    elif tag==9: # TAG_List
        data = None
    elif tag==10: # TAG_Compound
        data = None
    elif tag==11: # TAG_Int_Array
        data = None
    elif tag==12: # TAG_Long_Array
        data = None
    else:
        print(f"Invalid Tag: {tag}")
    #print(f"{tagType(tag)}: \"{tagName}\"\n\t{data}")
    

def csDecode(index):
    if index == 1:
        return "GZip"
    elif index == 2:
        return "Zlib"
    elif index == 3:
        return "Uncompressed"
    elif index == 4:
        return "LZ4"
    elif index == 127:
        return "Custom"
    else:
        return "Unsupported"

for c in range(1024):
    f.seek(c*4, 0)
    depth = 0
    header_index = c
    offset = intread(f,3)*(4*kib)
    sector = intread(f,1)*(4*kib)
    if offset == 0 and sector == 0:
        continue
    else:
        print(f"Chunk #{header_index}: {offset}, {sector}KiB")
        f.seek(offset, 0)
        length = intread(f,4)
        compressionScheme = intread(f,1)
        if compressionScheme == 1: #GZip
            data = gzip.decompress(f.read(length-1))
        elif compressionScheme == 2: #Zlip
            data = zlib.decompress(f.read(length-1))
        print(f"\t{length} Bytes\n\tCompression {csDecode(compressionScheme)}")
        nbt = BytesIO(data)
        for i in range(4):
            decodeTag(nbt)
        #open('out.nbt', 'wb').write(data)
f.read(1024*4)
f.close()