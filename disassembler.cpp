/*  Omar Beltran
 *  SIC/XE Disassembler
 */

#include "disassembler.hpp"

int main(int argc, char *argv[]){
    
    //Return error and message if no object or symbol file inputted
    if(argc != 3){ 
	cout << "Missing input. Please enter file names\n" 
	<< "Example: ./dissem <OBJ FILE> <SYM FILE>\n";
    	return 1;
    }
    //get object and symbol file names with user input
    else{
        obj = argv[1];
        sym = argv[2];
    }

    string line; //for string of each line of the object file

    //open object and listing files
    objFile.open(obj);
    lisFile.open(lis);

    if(objFile.is_open()){
        //gets each line and sends it to their respective purpose
        while(getline(objFile,line)){
            if(line[0] == 'H') header(line); //go to header record function
            if(line[0] == 'T') text(line); //go to text record function
            if(line[0] == 'E') end(line); //go to end text function
        }
    }
    else {
        cout << "error: unable to open file " << obj << "\n";
    }

    objFile.close();
    lisFile.close();
    return 0;
}

void header(string line){

    //print starting address to lis file
    for(int i = 9; i<=12;i++) addrStr += line[i];
    lisFile << addrStr << "\t";

    //converts starting address string to integer for future reference
    addrInt = stoi(addrStr, nullptr, 16);

    //prints header name to sic file
    for(int i = 1; i<=6; i++){
        lisFile << line[i];
    }
    lisFile << "\tSTART\t";  
    lisFile << addrInt << "\n"; 

    //get the starting address 
    for(int i = 7; i < 13; i++){
        startAddr += line[i];
    }

    //get the ending address
    for(int i = 13; i < 19; i++){
        endAddr += line[i];
    }
}

void text(string line){
    //Get starting address of text record
    string textAddr;
    for(int i = 1; i < 7; i++){
        textAddr += line[i];
    }
    string tempA = "00";
    tempA += addrStr;
    //this will find and print all label RESW if the starting address of the text record does not equal the current address
    if(tempA != textAddr) resW(textAddr);

    //Get starting address (again) of text record and make that the current address
    textAddr = "";
    for(int i = 3; i<7; i++) textAddr += line[i];
    addrStr = textAddr;
    addrInt = stoi(addrStr,nullptr, 16);

    //current address in text record starting with at 0
    int count = 9;

    //string length of text record;
    string tLengthStr;
    tLengthStr = tLengthStr + line[7] + line[8];

    //integer length of text record
    int tLength = stoi(tLengthStr, nullptr, 16);
    tLength *= 2;    

    //runs as long as it is within text record length
    while(count < tLength+9){

        //print current address of text record
        string tempAddr;
        if(addrStr.length() == 1) tempAddr = "000";
        else if(addrStr.length() == 2) tempAddr = "00";
        else if(addrStr.length() == 3) tempAddr = "0";
        tempAddr += addrStr; //tempAddr used to compare to literals and labels
        addrStr = tempAddr; //current address string that is used to print at beginning of each line
        tempAddr = "00" + addrStr;

        symFile.open(sym);
        //print LTORG if there is a literal
        string symLine;
        string tempLiteralLine;
        if(symFile.is_open()){
            while(getline(symFile,symLine)){
                if(symLine[8] == '='){
                    for(int i = 21 ; i < symLine.length(); i++ ){
                        if(symLine[i] == ' ') continue;
                        else tempLiteralLine += symLine[i];
                    }
                }
                if(tempLiteralLine == tempAddr){
                    lisFile << "\t\t" << "LTORG" << "\n";
                    break;
                }
            } 
        }
        else cout << "error: unable to open file " << sym << "\n";
        
        lisFile << addrStr << "\t"; //print current address
        symFile.close();

        symFile.open(sym);
        int lit = 0; //whether literal is there or not
        int label = 0; //whether label is there or not
        int litAdd = 0; //length of literal to add to counter
        //see if address is a label or literal from symFile
        if(symFile.is_open()){
            string symLine;
            while(getline(symFile,symLine)){
                string tempLabelLine;
                string tempLiteralLine;
                //gets the label address if there is one
                if(symLine[8] != '=' && symLine[1] != '='){
                    for(int i = 8 ; i < 14; i++ ){
                        tempLabelLine += symLine[i];
                    }
                }
                //gets literal address if there is one
                if(symLine[8] == '=' || symLine[1] == '='){
                    for(int i = 21; i<symLine.length(); i++){
                        if(symLine[i] == ' ') continue;
                        else tempLiteralLine += symLine[i];
                    }
                }

                //check if label is the current address in symFile
                if(tempLabelLine == tempAddr){
                    tempLabelLine = "";
                    label = 1;
                    //get label name and print to listing file
                    for(int i = 0; i < 6; i++){
                        tempLabelLine += symLine[i];
                    }
                    lisFile << tempLabelLine;
                    break;
                }
                //check literal is the current address in symFile
                else if(tempLiteralLine == tempAddr){
                    tempLiteralLine = "";
                    string litLength;
                    //get literal name and print to listing file
                    if(symLine[8]=='='){
                        for(int i = 19; i <22; i++){
                            litLength += symLine[i];
                        }
                        for(int i = 8; i < 18; i++){
                            tempLiteralLine += symLine[i];
                        }
                    }
                    else if(symLine[1] == '='){
                        for(int i = 12; i < 15; i++){
                            litLength += symLine[i];
                        }
                        for(int i = 1; i < 12; i++){
                            tempLiteralLine += symLine[i];
                        }
                    }
                    litAdd = stoi(litLength,nullptr,10);
                    addrInt += (stoi(litLength,nullptr,10)) / 2; 
                    lit = 1;
                    //print literal line
                    lisFile << "*" << "\t" << tempLiteralLine << "\t\t";
                    for(int i = 3; tempLiteralLine[i] != '\''; i++){
                        lisFile<< tempLiteralLine[i];
                    }
                    break;
                }
            }
            lisFile << "\t"; //print tab for formatting purposes
        }
        else cout << "error: unable to open file " << sym << "\n";
        symFile.close();

        string opNum; //opcode
        opNum = line[count];
        
        // Removes the two rightmost bits for the opcode
        if(line[count+1] == '1' || line[count+1] == '2' || line[count+1] == '3' ) opNum += "0";
        else if(line[count+1] == '5' || line[count+1] == '6' || line[count+1] == '7' ) opNum += "4";
        else if(line[count+1] == '9' || line[count+1] == 'A' || line[count+1] == 'B' ) opNum += "8";
        else if(line[count+1] == 'D' || line[count+1] == 'E' || line[count+1] == 'F' ) opNum += "C";
        else opNum += line[count+1];

        //only when it is not a literal
        if(lit == 0){
            //iterate through mnemonic table and check if there is a matching opcode
            for(int i = 0; i < sizeof(table)/sizeof(*table); i++){
                if(table[i].op == opNum){
                    int add2 = 0; //adds two half bytes to count if format 4
                    int x = 0; //X flag 
                    //format 4 check
                    if(line[count+2] == '1' || line[count+2] == '9'){
                        lisFile << "+";
                        add2 = 2;
                    }
                    //displacement or address from object code; 
                    string tempStr;  
                    for(int i = 0; i < 3+add2; i++){
                        tempStr += line[count+i+3];
                    } 
                    //change Base register
                    if(opNum == "68") {
                        base = tempStr;   
                    }
                    //change X register
                    else if(opNum == "04"){
                        xValue = tempStr;
                    }
                    
                    int programCounter = addrInt + add2 + table[i].length; //get program counter of current instruction
                    lisFile << table[i].name; //print mnemonic name

                    //detemines if instruction is simple, immediate, or indirect and print either #, @, or nothing.
                    if(line[count+1] == '1' || line[count+1] == '5' || line[count+1] == '9' || line[count+1] == 'D') lisFile << "\t#";
                    else if(line[count+1] == '2' || line[count+1] == '6' || line[count+1] == 'A' || line[count+1] == 'E') lisFile << "\t@";
                    else lisFile << "\t";  

                    //print register for mnemonic CLEAR  
                    if(opNum == "B4"){
                        if(line[count+2] == '0') lisFile << "A";
                        else if(line[count+2] == '1') lisFile << "X";
                        else if(line[count+2] == '2') lisFile << "L";
                        else if(line[count+2] == '3') lisFile << "B";
                        else if(line[count+2] == '4') lisFile << "S";
                        else if(line[count+2] == '5') lisFile << "T";
                        else if(line[count+2] == '6') lisFile << "F";
                    }
                    else if(opNum == "4C"){
                        //skips printing to RSUB instruction
                    }
                    //print displacement constant
                    else if(line[count+2] == '0'){
                        int outputInt = stoi(tempStr,nullptr,16);
                        lisFile << outputInt;
                    }
                    //print label from address
                    else if(line[count+2]=='1'){
                        string targetAddr = "0";
                        if(tempStr.length() == 3) targetAddr += "00" + tempStr;
                        else if(tempStr.length() == 4) targetAddr += "0" + tempStr;
                        else if(tempStr.length() == 5) targetAddr += tempStr;
                        checkLabel(targetAddr);
                    }
                    //print label from address (PC)+disp
                    else if(line[count+2] == '2'){
                        int addr;
                        if(tempStr[0]== 'F'){
                            string temp = "F" + tempStr;
                            int disp = (65535-stoi(temp, nullptr,16)+1) * -1;
                            addr = (disp) + programCounter;
                        }
                        else addr = stoi(tempStr, nullptr,16) + programCounter;
                        stringstream ss;
                        ss << hex << addr;
                        string printStr = (ss.str());

                        //capitalize string
                        for(int i = 0; i < 4; i++){
                            if(printStr[i] >= 'a' && printStr[i] <= 'z') printStr[i] = toupper(printStr[i]);
                        }
                        string targetAddr = "00";
                        if(printStr.length() == 1) targetAddr += "000" + printStr;
                        else if(printStr.length() == 2) targetAddr += "00" + printStr;
                        else if(printStr.length() == 3) targetAddr += "0" + printStr;
                        else targetAddr += printStr;

                        checkLabel(targetAddr);
                    }
                    //print label from address (PC)+disp
                    else if(line[count+2] == '4'){
                        int addr;
                        addr = stoi(tempStr, nullptr,16) + stoi(base,nullptr,16);
                        stringstream ss;
                        ss << hex << addr;
                        string printStr = (ss.str());

                        //capitalize string
                        for(int i = 0; i < 4; i++){
                            if(printStr[i] >= 'a' && printStr[i] <= 'z') printStr[i] = toupper(printStr[i]);
                        }
                        string targetAddr = "00";
                        if(printStr.length() == 1) targetAddr += "000" + printStr;
                        else if(printStr.length() == 2) targetAddr += "00" + printStr;
                        else if(printStr.length() == 3) targetAddr += "0" + printStr;
                        else targetAddr += printStr;

                        checkLabel(targetAddr);
                    }
                    //print (x)+disp/addr + some other value (if necessary)
                    else if(line[count+2] == '8' || line[count+2] == '9' || line[count+2] == 'A' || line[count+2] == 'C'){
                        x = 1;
                        int addr;

                        //disp + (X)
                        if(tempStr[0]== 'F'){
                            string temp = "F" + tempStr;
                            int disp = (65535-stoi(temp, nullptr,16)+1) * -1;
                            addr = (disp) + stoi(xValue,nullptr,16);
                        }
                        else addr = stoi(tempStr, nullptr,16) + stoi(xValue,nullptr,16);

                        //(PC) + disp + (X)
                        if(line[count+2] == 'A') {
                            if(tempStr[0]== 'F'){
                                string temp = "F" + tempStr;
                                int disp = (65535-stoi(temp, nullptr,16)+1) * -1;
                                addr = (disp) + programCounter;
                            }
                            else addr += programCounter;
                        }
                        //(B) + disp + (X)
                        else if(line[count+2] == 'C') addr += stoi(base,nullptr,16);
                        //print displacement constant
                        else if(line[count+2] == '8') lisFile << addr;
                        //get label from address 
                        if(line[count+2] != '8'){
                            //conver int to hex string
                            stringstream ss;
                            ss << hex << addr;
                            string printStr = (ss.str());
                            //capitalize string
                            for(int i = 0; i < 4; i++){
                                if(printStr[i] >= 'a' && printStr[i] <= 'z') printStr[i] = toupper(printStr[i]);
                            }
                            //format hex address to be be length of 6
                            string targetAddr = "00";
                            if(printStr.length() == 1) targetAddr += "000" + printStr;
                            else if(printStr.length() == 2) targetAddr += "00" + printStr;
                            else if(printStr.length() == 3) targetAddr += "0" + printStr;
                            else targetAddr += printStr;

                            checkLabel(targetAddr); //print label if there is one
                        }
                        lisFile<< ",X"; //print ,X after label or constant
                    }

                    //print opcode
                    string tempRecord;
                    int addCount = (table[i].length*2)+add2;
                    for(int i = 0; i < addCount; ++i){
                        tempRecord += line[count+i]; 
                    }
                    if(tempLabel.length() != 6 || x != 1) lisFile <<"\t";
                    lisFile << "\t"<<tempRecord;

                    //print base and its label when mnemonic is LDB
                    if(opNum == "68") lisFile << "\n\t\tBASE\t" << tempLabel;

                    count += addCount; //update count of text record
                    addrInt += (table[i].length) + (add2/2); //update current address
                    break;          
                }
            }
            
        }
        else count += litAdd; //add literal length to count

        //converts int to hex string
        stringstream ss;
        ss << hex << addrInt;
        addrStr = (ss.str());
        lisFile << "\n"; 

        //capitalize all letters 
        for(int i = 0; i < 4; i++){
            if(addrStr[i] >= 'a' && addrStr[i] <= 'z') addrStr[i] = toupper(addrStr[i]);
        }
    }
}

void checkLabel(string targetAddr){
    //find label and print it
    symFile.open(sym);
    if(symFile.is_open()){
        string symLine;
        while(getline(symFile,symLine)){
            string tempLabelLine;
            string tempAddrLine;
            string tempLiteralLine;
            //gets label and it's associated address from symFile
            if(symLine[8] != '=' && symLine[1] != '='){
                for(int i = 0 ; i < 6; i++ ){
                    tempAddrLine += symLine[i+8];
                    tempLabelLine += symLine[i];
                }
            }
            //gets literal and it's associated address from symFile
            else if(symLine[8] == '=' || symLine[1] == '='){
                for(int i = 21; i<symLine.length(); i++){
                    if(symLine[i] == ' ') continue;
                    else tempLiteralLine += symLine[i];
                }
            }

            //check if label is the current address in symFile
            if(tempAddrLine == targetAddr){
                tempLabel = tempLabelLine;
                //remove all trailing whitespaces of label
                size_t found = tempLabel.find_last_not_of(" \t\f\v\n\r");
                if (found!=string::npos) tempLabel.erase(found+1);
                else tempLabel.clear(); 
                lisFile << tempLabel; //print label
                break;
            }
            //check it literal is the current address in symFile
            else if(tempLiteralLine == targetAddr){
                tempLiteralLine = "";
                if(symLine[8] == '='){
                    for(int i = 8; i < 18 || symLine[i] == ' '; i++){
                        tempLiteralLine += symLine[i];
                    }
                }
                else if(symLine[1] == '='){
                    for(int i = 1; i < 11 || symLine[i] == ' '; i++){
                        tempLiteralLine += symLine[i];
                    }
                }
                //remove all trailing whitespaces of label
                size_t found = tempLiteralLine.find_last_not_of(" \t\f\v\n\r");
                if (found!=string::npos) tempLiteralLine.erase(found+1);
                else tempLiteralLine.clear(); 
                lisFile << tempLiteralLine; //print literal
                break;
            }
        }
    }    
    symFile.close();
}

void resW(string lastAddr){
    string symLine;
    int pastAddr;
    int flag;
    symFile.open(sym);
    if(symFile.is_open()){
        while(getline(symFile,symLine)){
            string labelAddr;
            string label;
            int labelInt;
            //stop when reaching literals
            if(symLine[8] == 'L' || symLine == "\0") break;
            //goes to next line if not on labels yet
            else if(symLine[8] == 'V' || symLine[8] == '-') continue;
            else{
                //get label and it's address
                for(int i = 0; i < 6; i++){
                    label += symLine[i];
                }
                for(int i = 10; i < 14; i++){
                    labelAddr += symLine[i];
                }
                labelInt = stoi(labelAddr,nullptr,16);
            }
            //stops when label is great than target address
            if(labelInt >= stoi(lastAddr,nullptr,16)) break;
            //gets length of label is words (RESW)
            if(flag == 1 && labelInt >= addrInt){
                lisFile << (labelInt - pastAddr)/3 << "\n";
                addrInt = labelInt;
                //converts int to hex string
                stringstream ss;
                ss << hex << addrInt;
                addrStr = (ss.str());
            }
            //prints label address, label, and that it is RESW
            if(labelInt >= addrInt){
                lisFile << labelAddr << "\t" << label << "\tRESW\t";
                pastAddr = labelInt;
                flag = 1; //flag to help determine the label length for next iteration
            }
        }
    }
    symFile.close();
    
    lisFile << (stoi(lastAddr, nullptr, 16)-addrInt)/3 << "\n"; //print last label length
}

void end(string line){
    
    if(stoi(endAddr, nullptr, 16) > addrInt) resW(endAddr); //prints ending label RESW
    //prints end record with beginning label.
    lisFile << "\t\tEND\t" ;
    checkLabel(startAddr);
    lisFile << "\n";
}
