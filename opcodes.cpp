#include "headers.h"

#define OPCODE_ARRAY_SIZE 26

typedef struct opcode {
    std::string name;
    int value;
} opcode;

/** Opcode Array- Pre-determined values to use for each opcode */
opcode opcodes[OPCODE_ARRAY_SIZE] = {
        {"ADD",0x18},{"AND",0x40},{"COMP",0x28},{"DIV",0x24},
        {"J",0x3C},{"JEQ",0x30},{"JGT",0x34},{"JLT",0x38},
        {"JSUB",0x48},{"LDA",0x00},{"LDCH",0x50},{"LDL",0x08},
        {"LDX",0x04},{"MUL",0x20},{"OR",0x44},{"RD",0xD8},
        {"RSUB",0x4C},{"STA",0x0C},{"STCH",0x54},{"STL",0x14},
        {"STSW",0xE8},{"STX",0x10},{"SUB",0x1C},{"TD",0xE0},
        {"TIX",0x2C},{"WD",0xDC}
};

/**
 * Fetch OPCODE Value based upon a String
 * @param opcode
 * @return Value from opcodes[] if found, else -1.
 */
int getOpcodeValue(std::string opcode) {
    for(int i=0;i<OPCODE_ARRAY_SIZE;i++){
        if(opcodes[i].name==opcode){
            return opcodes[i].value;
        }
    }
    return -1;
}
bool isOpcode(std::string string) {
    return getOpcodeValue(string)!=-1;
}