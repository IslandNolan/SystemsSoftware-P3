#include "headers.h"

#define SYMBOL_TABLE_SIZE 100
#define SEGMENT_SIZE 10

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

void writeToLstFile(std::ofstream& file, const std::string& address, segment* segments, string opcode) {



    //Pass 2
}

void writeToObjFile(std::ofstream& oss, objectFileData fileData) {
        stringstream lineToWrite;
        switch(fileData.recordType){
            case 'H': {

                lineToWrite << "H";
                lineToWrite << setw(6) << std::left << fileData.programName;
                lineToWrite << setw(6) << std::right << setfill('0') << fileData.startAddress;
                lineToWrite << setw(6) << std::right << setfill('0') << fileData.programSize;
                oss << lineToWrite.str() << std::endl;
                break;
            }
            case 'T':{
                lineToWrite << "T";
                lineToWrite << setw(6) <<  std::right << setfill('0') << fileData.recordAddress;
                lineToWrite << setw(6) << std::right<< setfill('0') << fileData.recordByteCount;
                oss << lineToWrite.str() << std::endl;
                break;
            }
            case 'E':{
                lineToWrite << "E";
                lineToWrite << setw(6) << setfill('0') << std::right  << fileData.startAddress << std::endl;
                oss << lineToWrite.str() << endl;
                std::cout << lineToWrite.str() << std::endl;
                break;
            }
            case 'M':{




                //reserved for extra credit modification record.
                break;
            }
        }




    //Pass 2

}

void resetObjectFileData(objectFileData* ofd,address* addresses){
    ofd->recordAddress = addresses->current;
    ofd->recordEntryCount=0;
    ofd->recordByteCount=0;
    cout << "RESET OBJ" << std::endl;
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
void performPass1(struct symbol symbolTable[], const std::string& filename, address* addresses) {
    //cout << "\nSymbol Table Log\n----------------" << std::endl;
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
                addresses->start = current->third;
                addresses->current = current->third;
                continue;
            }
            else {
                int numBytes = getMemoryAmount(getDirectiveValue(current->second),current->third);
                addresses->increment = toHex(to_string(numBytes));
            }
        }
        else if(isOpcode(current->second)) {
            addresses->increment = toHex(to_string(3));
        }
        else{
            displayError(ILLEGAL_OPCODE_DIRECTIVE,current->second,lineNumber);
            exit(1);
        }
        int newValue = (stoi(toDec(addresses->current)) + stoi(toDec(addresses->increment)));
        if(newValue> stoi(toDec("0x8000"))) { displayError(OUT_OF_MEMORY, toHex(to_string(newValue)),lineNumber); exit(1); }

        if(!current->first.empty()) {
            checkDuplicates(symbolTable,current);
            insertSymbol(symbolTable,current->first,addresses->current);
        }
        addresses->current = toHex(to_string(newValue));
        delete(current);
    }
    //std::cout << std::endl;
    //displaySymbolTable(symbolTable);
    ifs.close();
    /*
    std::cout << "\n\nAssembly Summary - "+filename+"\n----------------\n"
              << setw(20) << "Starting Address: " << addresses->start << endl
              << setw(20) << " Ending Address:  "<< addresses->current << endl
              << setw(20) << " Size (bytes):  " << (stoi(toDec(addresses->current))-stoi(toDec(addresses->start))) << endl;
    */
    std::cout << "Finished Pass 1 for SIC file: "+filename << endl;
}

/**
 * Perform the SIC Assmenly Pass 2 on the specified file, generates two additional files, filename.lst, and filename.obj
 * @param symbolTable
 * @param filename
 * @param addresses
 */
void performPass2(struct symbol symbolTable[],const std::string& filename,address* addresses){

    std::cout << "Begin Pass 2--" << std::endl;
    objectFileData objectData = { 0, { 0x0 }, "", "0", "0x0", 0, {  }, 0, ' ', "0x0" };

    //Create both .lst, and .obj files.
    ofstream lstFile(createFile(filename,".lst"));
    ofstream objFile(createFile(filename,".obj"));
    ifstream ifs(filename);

    if(!ifs.is_open()) { displayError(FILE_NOT_FOUND,filename,-1); exit(1); }
    //Instructions say it's not necessary to check for open failures for .lst, and .obj files.

    std::string currentLine;
    int lineNumber;
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
                objectData.programSize = toHex(to_string(stoi(toDec(addresses->current))-stoi(toDec(addresses->start))));
                addresses->current = addresses->start;
                writeToObjFile(objFile,objectData);
                //writeToLstFile(lstFile,objectData);
            }
            else if(isEndDirective(current->second)){
                if(objectData.recordByteCount>0) {
                    writeToObjFile(objFile,objectData);
                    resetObjectFileData(&objectData,addresses);
                }
                objectData.recordType='E';
                writeToObjFile(objFile,objectData);
                //writeToLstFile()
            }
            else if(isReserveDirective(current->second)){
                if(objectData.recordByteCount>0){
                    writeToObjFile(objFile,objectData);
                    resetObjectFileData(&objectData,addresses);
                }
                //writeToLstFile()
                addresses->increment = toHex(to_string(getMemoryAmount(getDirectiveValue(current->second),current->third)));
                objectData.recordAddress = toHex((to_string(stoi(toDec(objectData.recordAddress))+stoi(toDec(addresses->increment)))));

            }
            else {
                //Must be data directive.
                objectData.recordType='T';
                addresses->increment = toHex(to_string(getMemoryAmount(getDirectiveValue(current->second),current->third)));
                if(MAX_RECORD_BYTE_COUNT-stoi(toDec(addresses->increment))<objectData.recordByteCount){
                    //Dump because not enough room
                    writeToObjFile(objFile,objectData);
                    resetObjectFileData(&objectData,addresses);
                }
                string wordValue = getByteWordValue(getDirectiveValue(current->second),current->third);
                struct recordEntry entry = recordEntry { addresses->increment,wordValue };
                objectData.recordEntryCount++;
                objectData.recordEntries[objectData.recordEntryCount] = entry;
                objectData.recordByteCount+=stoi(toDec(addresses->increment));
                //writeToLstFile(lstFile,addresses->current,current,wordValue);
            }
        }


        //Requirement so we don't leak memory.
        delete(current);
    }
}

int main(int argc, char* argv[]) {

    if(argc<2) { displayError(MISSING_COMMAND_LINE_ARGUMENTS,std::string("Missing Args"),-1); exit(1); }
    address addresses = { "", "", "" };



    auto* symbolTable = (symbol*) calloc(sizeof(struct symbol),100);

    performPass1(symbolTable,argv[1],&addresses);

    performPass2(symbolTable,argv[1],&addresses);
}
