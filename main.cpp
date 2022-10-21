#include "headers.h"

#define SYMBOL_TABLE_SIZE 100

using namespace std;

/**
 * @author Nolan Boner - N01440422
 * Splits, and Prepares Segments for the SIC Instruction from a standard String object.
 * Note: Ignores any tokens after the 3rd.
 *
 * @param statement String to process, expects 3 words.
 * @return a pointer to a typedef struct segment.
 */
segment* prepareSegments(std::string statement) {
    auto* temp = new segment();

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
    return temp;
}

void writeToLstFile(std::ofstream& oss, int address, segment* segments, int opcode) {
    std::stringstream lineToWrite;
    if(isDirective(segments->second)){
        if(isStartDirective(segments->second) || isEndDirective(segments->second) || isReserveDirective(segments->second)){
            lineToWrite << std::left << setw(8) << std::hex << address;
            lineToWrite << std::left << setw(8) << segments->first;
            lineToWrite << std::left << setw(8) << segments->second;
            lineToWrite << std::left << segments->third;
        }
        else if(segments->second=="BYTE" || segments->second=="WORD"){
            lineToWrite << std::left << setw(8) << std::hex << address;
            lineToWrite << std::left << setw(8) << segments->first;
            lineToWrite << std::left << setw(8) << segments->second;
            lineToWrite << std::left << setw(12) << segments->third;
            lineToWrite << std::right << setfill('0') << std::hex << setw((segments->third.find("X'")!=std::string::npos) ? 2 : 6) << opcode;
        }
    }
    else if(isOpcode(segments->second)){
        lineToWrite << std::left << setw(8) << std::hex << address;
        lineToWrite << std::left << setw(8) << segments->first;
        lineToWrite << std::left << setw(8) << segments->second;
        lineToWrite << std::left << setw(12) << segments->third;
        lineToWrite << std::right << setfill('0') << std::hex << setw((segments->third.find("X'")!=std::string::npos) ? 2 : 6) << opcode;
    }
    string res = lineToWrite.str();
    std::transform(res.begin(),res.end(),res.begin(),::toupper);
    oss << res << std::endl;
}

void writeToObjFile(std::ofstream& oss, objectFileData fileData) {
        stringstream lineToWrite;
        switch(fileData.recordType){
            case 'H': {
                lineToWrite << "H";
                lineToWrite << setw(6) << std::left << fileData.programName;
                lineToWrite << setw(6) << std::right << setfill('0') << std::hex << fileData.startAddress;
                lineToWrite << setw(6) << std::hex << fileData.programSize;
                std::cout << std::resetiosflags(std::ios::showbase);
                break;
            }
            case 'T':{
                lineToWrite << "T";
                lineToWrite << setw(6) <<  std::right << setfill('0') << std::hex << fileData.recordAddress;
                lineToWrite << setw(2) <<  std::right << setfill('0') << std::hex << fileData.recordByteCount;
                std::cout << std::resetiosflags(std::ios::showbase);
                for(int i=0;i<fileData.recordEntryCount;i++){
                    lineToWrite << std::hex << std::setw(fileData.recordEntries[i].numBytes==1 ? 2 : 6) << std::right << fileData.recordEntries[i].value;
                }
                break;
            }
            case 'E':{
                lineToWrite << "E";
                lineToWrite << setw(6) << setfill('0') << std::right  << std::hex << fileData.startAddress << std::endl;
                std::cout << std::resetiosflags(std::ios::showbase);
                break;
            }
            case 'M': {
                //reserved for extra credit modification record.
                break;
            }
        }
        string recordUpper = lineToWrite.str();
        std::transform(recordUpper.begin(),recordUpper.end(),recordUpper.begin(),::toupper);
        oss << recordUpper << std::endl;
}

void resetObjectFileData(objectFileData* ofd,address* addresses){
    ofd->recordAddress = addresses->current;
    ofd->recordEntryCount=0;
    ofd->recordByteCount=0;
}

std::string createFile(const std::string& filename,const std::string& ext){
    string modified = filename;
    modified.erase(filename.size()-4,4);
    modified.append(ext);

    //Delete old files, and just open new ones when the oss is created.
    remove(modified.c_str());

    //Use constructor to create oss for the file, then delete it so no mem leaks.
    auto* oss = new ofstream(modified,ios_base::app);
    if(oss->good()) {
        oss->close();
        delete(oss);
        return modified;
    }
    //an error has occurred if we reach here.
    return "";
}

/**
 * Perform the SIC Assembly Pass 1 on the specified file using the declared objects.
 * @param symbolTable Reference to table to use.
 * @param filename File to parse
 * @param addresses Current Addresses for the instruction
 */
void performPass1(struct symbol symbolTable[], std::string filename, address* addresses) {
    std::ifstream ifs(filename);
    if(!ifs.is_open()) { displayError(FILE_NOT_FOUND,filename); exit(1); }
    std::string currentLine;
    int lineNumber=0;
    while(getline(ifs,currentLine)) {
        lineNumber++;
        if (currentLine[0] == '#') { continue; }
        if (currentLine[0] < 32) {
            displayError(BLANK_RECORD,"",lineNumber);
            exit(1);
        }
        segment *current = prepareSegments(currentLine);

        if(isDirective(current->first) || isOpcode(current->first)) {
            displayError(ILLEGAL_SYMBOL,current->first,lineNumber);
            exit(1);
        }
        else if(isDirective(current->second)) {
            if(isStartDirective(current->second)){
                addresses->start = std::strtoull(("0x"+current->third).c_str(),nullptr,16);
                addresses->current = std::strtoull(("0x"+current->third).c_str(),nullptr,16);
                continue;
            }
            else {
                addresses->increment = getMemoryAmount(getDirectiveValue(current->second),current->third);
            }
        }
        else if(isOpcode(current->second)) {
            addresses->increment = 0x3;
        }
        else{
            displayError(ILLEGAL_OPCODE_DIRECTIVE,current->second,lineNumber);
            exit(1);
        }
        int newValue = addresses->current + addresses->increment;
        if(newValue > 0x8000) { displayError(OUT_OF_MEMORY, to_string(newValue),lineNumber); exit(1); }

        if(!current->first.empty()) {
            checkDuplicates(symbolTable,current);
            insertSymbol(symbolTable,current->first,addresses->current);
        }
        addresses->current = newValue;
        delete(current);
    }
    ifs.close();

    /*
    std::cout << std::endl;
    displaySymbolTable(symbolTable);
    std::cout << "\n\nAssembly Summary - "+filename+"\n----------------\n"
              << setw(20) << "Starting Address: " << std::hex << addresses->start << std::dec << endl
              << setw(20) << " Ending Address:  "<< std::hex << addresses->current << std::dec << endl
              << setw(20) << " Size (bytes):  " << addresses->current-addresses->start << std::resetiosflags(std::ios::showbase) << std::endl;
    */
}

/**
 * Perform the SIC Assmenly Pass 2 on the specified file, generates two additional files, filename.lst, and filename.obj
 * @param symbolTable
 * @param filename
 * @param addresses
 */
void performPass2(struct symbol symbolTable[],const std::string& filename,address* addresses){

    objectFileData objectData = { 0, { 0x0 }, "", 0, 0x0, 0, {  }, 0, ' ', 0x0 };

    //Create both .lst, and .obj files.
    ofstream lstFile(createFile(filename,".lst"));
    ofstream objFile(createFile(filename,".obj"));
    ifstream ifs(filename);

    if(!ifs.is_open()) { displayError(FILE_NOT_FOUND,filename,-1); exit(1); }
    //Instructions say it's not necessary to check for open failures for .lst, and .obj files.

    std::string currentLine;
    int lineNumber=0;
    while(getline(ifs,currentLine)) {
        lineNumber++;
        if (currentLine[0] == '#') { continue; }
        if (currentLine[0] < 32) {
            displayError(BLANK_RECORD, "", lineNumber);
            exit(1);
        }
        segment *current = prepareSegments(currentLine);
        if(isDirective(current->second)){
            if(isStartDirective(current->second)){
                //Everything here is stored as a string hex.
                objectData.recordType='H';
                objectData.programName=current->first;
                objectData.startAddress=addresses->start;
                objectData.recordAddress=addresses->start;
                objectData.programSize = addresses->current-addresses->start;
                addresses->current = addresses->start;
                writeToObjFile(objFile,objectData);
                writeToLstFile(lstFile,addresses->current,current,BLANK_INSTRUCTION);
            }
            else if(isEndDirective(current->second)){
                if(objectData.recordByteCount>0) {
                    writeToObjFile(objFile,objectData);
                    resetObjectFileData(&objectData,addresses);
                }
                objectData.recordType='E';
                writeToObjFile(objFile,objectData);
                writeToLstFile(lstFile,addresses->current,current,BLANK_INSTRUCTION);
            }
            else if(isReserveDirective(current->second)){
                if(objectData.recordByteCount>0){
                    writeToObjFile(objFile,objectData);
                    resetObjectFileData(&objectData,addresses);
                }
                writeToLstFile(lstFile,addresses->current,current,BLANK_INSTRUCTION);
                addresses->increment = getMemoryAmount(getDirectiveValue(current->second),current->third);
                objectData.recordAddress+=addresses->increment;

            }
            else {
                //Must be data directive
                addresses->increment = getMemoryAmount(getDirectiveValue(current->second),current->third);
                if(MAX_RECORD_BYTE_COUNT-addresses->increment<objectData.recordByteCount){
                    //Dump because not enough room
                    writeToObjFile(objFile,objectData);
                    resetObjectFileData(&objectData,addresses);
                }
                objectData.recordType='T';

                int wordValue = getByteWordValue(getDirectiveValue(current->second),current->third);
                struct recordEntry entry = recordEntry { addresses->increment,wordValue };
                objectData.recordEntries[objectData.recordEntryCount] = entry;
                objectData.recordByteCount = objectData.recordByteCount+addresses->increment;
                objectData.recordEntryCount++;
                writeToLstFile(lstFile,addresses->current,current,wordValue);
            }
        }
        else if(isOpcode(current->second)){
            objectData.recordType='T';
            if(MAX_RECORD_BYTE_COUNT-3<objectData.recordByteCount){
                //Dump and Start new Record.
                writeToObjFile(objFile,objectData);
                resetObjectFileData(&objectData,addresses);
            }
            int value = getOpcodeValue(current->second)*OPCODE_MULTIPLIER;
            if(current->second!="RSUB"){
                if(current->third.find(",X")!=std::string::npos){
                    value+=X_MULTIPLER;
                    string trimmed = current->third;
                    trimmed.erase(trimmed.size()-2,2);
                    int response = getSymbolAddress(symbolTable,trimmed);
                    value+=response;
                }
                else {
                    int response = getSymbolAddress(symbolTable,current->third);
                    if(response==-1){
                        displayError(UNDEFINED_SYMBOL,current->third,lineNumber);
                        exit(1);
                    }
                    value+=response;
                }
            }

            struct recordEntry record = recordEntry { 3,value };
            objectData.recordEntries[objectData.recordEntryCount]=record;
            objectData.recordEntryCount++;
            objectData.recordByteCount+=3;
            writeToLstFile(lstFile,addresses->current,current,value);
            addresses->increment = 0x3;
        }
        addresses->current+=addresses->increment;


        //Requirement so we don't leak memory.
        delete(current);
    }
}

int main(int argc, char* argv[]) {

    if(argc<2) { displayError(MISSING_COMMAND_LINE_ARGUMENTS,std::string("Missing Args"),-1); exit(1); }
    address addresses = { 0x0, 0x0, 0x0 };

    auto* symbolTable = (symbol*) calloc(sizeof(struct symbol),100);

    performPass1(symbolTable,argv[1],&addresses);

    performPass2(symbolTable,argv[1],&addresses);
}
