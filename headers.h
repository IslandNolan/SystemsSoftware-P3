#pragma once

#include <iomanip>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <iosfwd>

#define NAME_SIZE 7
#define BLANK_INSTRUCTION 0x000000
#define OPCODE_MULTIPLIER 0x10000
#define OUTPUT_BUF_SIZE 70
#define RSUB_INSTRUCTION 0x4C0000
#define X_MULTIPLER 0x8000
#define MAX_RECORD_BYTE_COUNT 30

enum errors {
	BLANK_RECORD, DUPLICATE, FILE_NOT_FOUND, ILLEGAL_OPCODE_DIRECTIVE, ILLEGAL_SYMBOL,
	MISSING_COMMAND_LINE_ARGUMENTS, OUT_OF_MEMORY, OUT_OF_RANGE_BYTE, OUT_OF_RANGE_WORD, UNDEFINED_SYMBOL
};

typedef struct address {
	int start;
	int current;
	int increment;
} address;

typedef struct segment {
	std::string first;
	std::string second;
	std::string third;
} segment;

typedef struct symbol {
	std::string name;
	int address=0x0;
    struct symbol* next=nullptr;
} symbol;

typedef struct recordEntry {
    int numBytes;
    int value;
} recordEntry;

typedef struct objectFileData {
    int modificationCount; // Used for extra credit only
    int modificationEntries[60]; // Used for extra credit only
    std::string programName; // Used for H and M records only
    int programSize; // Used for H record only
    int recordAddress; // Used for T records only
    int recordByteCount; // Used for T records only
    recordEntry recordEntries[60]; // Used to store T record data
    int recordEntryCount; // Used for T records only
    char recordType; // H, T, E or M
    int startAddress; // Used for H and E records only
} objectFileData;

// directives.c functions
int getMemoryAmount(int directiveType, std::string string);
bool isDirective(std::string str);
int getDirectiveValue(std::string str);
bool isStartDirective(std::string str);
int getByteWordValue(int directiveType, std::string str);
bool isDataDirective(std::string str);
bool isEndDirective(std::string str);
bool isReserveDirective(std::string str);

// errors.c functions
void displayError(int errorType, std::string errorInfo,int lineNumber);
void displayError(int errorType, std::string errorInfo);

// opcodes.c functions
int getOpcodeValue(std::string opcode);
bool isOpcode(std::string string);

// symbols.c functions
int computeHash(std::string str);
void checkDuplicates(struct symbol symbolTable[],struct segment* current);
void displaySymbolTable(struct symbol symbolTable[]);
void insertSymbol(struct symbol symbolTable[], const std::string& symbolName, int symbolAddress);
int getSymbolAddress(struct symbol symbolArray[], std::string str);

