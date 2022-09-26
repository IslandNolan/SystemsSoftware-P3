#include "headers.h"

#define COMMENT 35
#define INPUT_BUF_SIZE 60
#define SYMBOL_TABLE_SIZE 100

void performPass1(struct symbol* symbolTable[], char* filename, address* addresses) {
	
}

/**
 * Splits, and Prepares Segments for the SIC Instruction from a String object.
 *
 * @param statement String to process, expects 3 words.
 * @return a pointer to a typedef struct segment.
 */
segment* prepareSegments(std::string statement) {
    auto* temp = (segment*) malloc(sizeof(segment));
    std::stringstream iss(statement);
    std::string current;
    for(int i=0;i<3;i++){
        if((i==0 && statement[0]==' ')) { continue; }
        else {
            iss >> current;
            switch(i){
                case 0:
                    temp->first=current;
                    break;
                case 1:
                    temp->second=current;
                    break;
                case 2:
                    temp->third=current;
                    break;
                default: break;
            }
        }
    }
    std::cout << std::left << std::setw(SEGMENT_SIZE) << temp->first << std::left << std::setw(SEGMENT_SIZE) << temp->second << std::left << std::setw(SEGMENT_SIZE) << temp->third << std::endl;
    return temp;
}
int main(int argc, char* argv[]) {
    if(argc<2) { displayError(MISSING_COMMAND_LINE_ARGUMENTS,std::string("Missing Args"),-1); exit(1); }
    std::ifstream ifs(argv[1]);
    if(!ifs.is_open()) { displayError(FILE_NOT_FOUND,argv[0],-1); exit(1); }
    else {
        address addresses = { 0x00, 0x00, 0x00 };
        std::string currentLine;
        int lineNumber=0;
        while(getline(ifs,currentLine)){
            lineNumber++;
            if(currentLine[0]=='#') { continue; }
            prepareSegments(currentLine);
            //insert into symbol table here.


        }
    }
    //print table here.
}
