#include<iostream>
#include<fstream>
#include<string>
#include<map>
#include<vector>

using namespace std;

map<string, int> initSymbolTable(){
    map<string, int> table;

    table["SP"]     = 0;
    table["LCL"]    = 1;
    table["ARG"]    = 2;
    table["THIS"]   = 3;
    table["THAT"]   = 4;
    table["SCREEN"] = 16384;
    table["KBD"]    = 24576;

    for(int i = 0; i < 16; i++){
        table["R"+to_string(i)] = i;
    }

    return table;
}

string getComp(string comp){
    map<string, string> compTable;
    compTable["0"]   = "0101010";
    compTable["1"]   = "0111111";
    compTable["-1"]  = "0111010";
    compTable["D"]   = "0001100";
    compTable["A"]   = "0110000";
    compTable["M"]   = "1110000";
    compTable["!D"]  = "0001101";
    compTable["!A"]  = "0110001";
    compTable["!M"]  = "1110001";
    compTable["-D"]  = "0001111";
    compTable["-A"]  = "0110011";
    compTable["-M"]  = "1110011";
    compTable["D+1"] = "0011111";
    compTable["A+1"] = "0110111";
    compTable["M+1"] = "1110111";
    compTable["D-1"] = "0001110";
    compTable["A-1"] = "0110010";
    compTable["M-1"] = "1110010";
    compTable["D+A"] = "0000010";
    compTable["D+M"] = "1000010";
    compTable["D-A"] = "0010011";
    compTable["D-M"] = "1010011";
    compTable["A-D"] = "0000111";
    compTable["M-D"] = "1000111";
    compTable["D&A"] = "0000000";
    compTable["D&M"] = "1000000";
    compTable["D|A"] = "0010101";
    compTable["D|M"] = "1010101";
    return compTable[comp];
}

string getDest(string dest){
    map<string, string> destTable;
    destTable[""]    = "000";
    destTable["M"]   = "001";
    destTable["D"]   = "010";
    destTable["MD"]  = "011";
    destTable["A"]   = "100";
    destTable["AM"]  = "101";
    destTable["AD"]  = "110";
    destTable["AMD"] = "111";
    return destTable[dest];
}

string getJump(string jump){
    map<string,string> jumpTable;
    jumpTable[""]    = "000";
    jumpTable["JGT"] = "001";
    jumpTable["JEQ"] = "010";
    jumpTable["JGE"] = "011";
    jumpTable["JLT"] = "100";
    jumpTable["JNE"] = "101";
    jumpTable["JLE"] = "110";
    jumpTable["JMP"] = "111";
    return jumpTable[jump];
}

string parseCInstruction(string line){
    string dest = "";
    string comp = "";
    string jump = "";

    int eqPos = line.find('=');
    if(eqPos!=string::npos){
        dest = line.substr(0,eqPos);
        line = line.substr(eqPos+1);
    }

    int scPos = line.find(';');
    if(scPos!=string::npos){
        comp = line.substr(0,scPos);
        jump = line.substr(scPos+1);
    }

    else{
        comp = line;
    }

    return "111"+getComp(comp)+getDest(dest)+getJump(jump);
}

vector<string> readFile(string fileName){
    vector<string> lines;
    ifstream file(fileName);
    string line;

    while(getline(file,line)){
        lines.push_back(line);
    }

    file.close();
    return lines;
}

string cleanLine(string line){
    int commentPos = line.find("//");
    if(commentPos != string::npos){
        line = line.substr(0,commentPos);
    }

    string result = "";
    for(char c : line){
        if(c != ' ' && c != '\t' && c != '\r'){
            result += c;
        }
    }
    return result;
}

string toBinary(int value){
    string result = "";
    for(int i = 15; i >= 0; i--){
        result += ((value >> i)& 1) ? "1" : "0";
    }
    return result;
}

//firstPass(labels)
void firstPass(vector<string>& lines,map<string, int>& symbolTable){
    int lineNumber = 0;

    for(string line : lines){
        string cleaned = cleanLine(line);

        if(cleaned.length()==0) continue;
        if(cleaned[0] == '('){
            string label = cleaned.substr(1,cleaned.length()-2);
            symbolTable[label] = lineNumber;
        }else{
            lineNumber++;
        }
    }
}

void secondPass(vector<string>&lines, map<string, int>& symbolTable, string outputFile){
    ofstream outFile(outputFile);
    int varAddress = 16;

    for(string line : lines){
        string cleaned = cleanLine(line);

        if(cleaned.length()==0)continue;
        if(cleaned[0] == '(')continue;

        if(cleaned[0] == '@'){
            string symbol = cleaned.substr(1);
            int value;


            if(isdigit(symbol[0])){
                value = stoi(symbol);
            }else if(symbolTable.count(symbol) > 0){
                value = symbolTable[symbol];
            }else{
                symbolTable[symbol] = varAddress;
                value = varAddress;
                varAddress++;
            }

            outFile<<toBinary(value)<<endl;
        }
        else{
            outFile<<parseCInstruction(cleaned)<<endl;
        }
    }
    outFile.close();
}

int main(int argc, char* argv[]){
    if(argc!=2){
        cout<<"Usage: assembler <filename.asm>"<<endl;
        return 1;
    }

    string inputFile = argv[1];
    map<string, int> symbolTable = initSymbolTable();

    
    vector<string> lines = readFile(inputFile);


    firstPass(lines,symbolTable);


    string outputFile = inputFile;
    outputFile.replace(outputFile.find(".asm"),4,".hack");
    secondPass(lines, symbolTable, outputFile);
    cout<<"Done! Output: "<<outputFile<<endl;
    
    return 0;
}
