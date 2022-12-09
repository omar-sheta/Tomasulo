#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<vector>
#include<map>

using namespace std;


struct instruction {
    string type = ""; // instruction type
    int index = -1; // index in program
    int cycle = -1; // cycle numbers needed
    string label = ""; // empty string if no label
    string rs1,rs2,rd = ""; // registers if exist
    int imm =0; // immediate if exist
    int offset = 0; // offset if exist
    bool isLabel = 0; // to not confuse labels with instructions
    
};


// read words and store in 2d vector of strings
void read_file(string filename, vector<vector<string>> &words) {
    ifstream file(filename);
    string line;
    while(getline(file, line)) {
        stringstream ss(line);
        string word;
        vector<string> temp;
        while(ss>>word) {
            // check if there is comma and remove it
            if(word[word.size()-1] == ',') {
                word = word.substr(0, word.size()-1);
            }
            temp.push_back(word);
        }
        words.push_back(temp);
    }
}


// program is a 2d vector of strings containing the instructions and labels if exist
// build vector of instructions structs
void parse_instructions(vector<vector<string>> program, vector<instruction> &instructions) 
{
    

for (int i=0; i<program.size(); i++) {


        
        // L1: ADD rA, rB, rC label instruction is the same as ADD rA, rB, rC but with a label
        // check if the first word is a label
        if(program[i][0][program[i][0].size()-1] == ':') {
            // if it is a label, remove the colon and store it in the label field of the instruction
            instruction temp;
            temp.label = program[i][0].substr(0, program[i][0].size()-1);
            temp.isLabel = 1;
            // check if the next word is an instruction
            if(program[i][1] == "ADD" || program[i][1] == "NOR" || program[i][1] == "MUL") {
                temp.type = program[i][1];
                temp.index = i;
                if(program[i][1] == "ADD") {
                    temp.cycle = 2;
                }
                else if(program[i][1] == "NOR") {
                    temp.cycle = 1;
                }
                else if(program[i][1] == "MUL") {
                    temp.cycle = 8;
                }
                temp.rd = program[i][2];
                temp.rs1 = program[i][3];
                temp.rs2 = program[i][4];
                instructions.push_back(temp);
            } else if (program[i][1] == "ADDI"){
                // for immediate, rd is the register to store the result and rs1 is the register to add to the immediate
                temp.type = program[i][1];
                temp.index = i;
                temp.cycle = 2;
                temp.rd = program[i][2]; 
                temp.rs1 = program[i][3];
                temp.imm = stoi(program[i][4]);
                instructions.push_back(temp);
            } else if(program[i][1]=="NEG"){ // NEG rA, rB
                temp.type = program[i][1];
                temp.index = i;
                temp.cycle = 1;
                temp.rd = program[i][2]; // negate rB and store in rA -> rA = -rB
                temp.rs1 = program[i][3];
                instructions.push_back(temp);
            }
        } else
        if(program[i][0] == "ADD" || program[i][0] == "NOR" || program[i][0] == "MUL") {
            instruction temp;
            temp.type = program[i][0];
            temp.index = i;
            if(program[i][0] == "ADD") {
                temp.cycle = 2;
            }
            else if(program[i][0] == "NOR") {
                temp.cycle = 1;
            }
            else if(program[i][0] == "MUL") {
                temp.cycle = 8;
            }

            temp.rd = program[i][1];
            temp.rs1 = program[i][2];
            temp.rs2 = program[i][3];
            instructions.push_back(temp);
        } else if (program[i][0] == "ADDI"){
            // for immediate, rd is the register to store the result and rs1 is the register to add to the immediate
            instruction temp;
            temp.type = program[i][0];
            temp.index = i;
            temp.cycle = 2;
            temp.rd = program[i][1]; 
            temp.rs1 = program[i][2];
            temp.imm = stoi(program[i][3]);
            instructions.push_back(temp);
        } else if(program[i][0]=="NEG"){ // NEG rA, rB
            instruction temp;
            temp.type = program[i][0];
            temp.index = i;
            temp.cycle = 1;
            temp.rd = program[i][1]; // negate rB and store in rA -> rA = -rB
            temp.rs1 = program[i][2];
            instructions.push_back(temp);


        }else if(program[i][0] == "JAL") {//JAL label
            instruction temp;
            temp.type = program[i][0];
            temp.index = i;
            temp.cycle = 1;
            temp.label = program[i][1];
            instructions.push_back(temp);


        }else if(program[i][0] == "RET") { //RET
            instruction temp;
            temp.type = program[i][0];
            temp.index = i;
            temp.cycle = 1;
            instructions.push_back(temp);

        }else if(program[i][0] == "BEQ") { //BEQ rA, rB, label
            instruction temp;
            temp.type = program[i][0];
            temp.index = i;
            temp.cycle = 1;
            temp.rs1 = program[i][1];
            temp.rs2 = program[i][2];
            temp.label = program[i][3];
            instructions.push_back(temp);
        }else if(program[i][0] == "LOAD") { //LOAD x1, offset(x2) load from memory address rB+offset to rA  
                                            // Make sure to use rd and rs1 for this instruction

            instruction temp;
            temp.type = program[i][0];
            temp.index = i;
            temp.cycle = 3;
            temp.rd = program[i][1];
            temp.offset = stoi(program[i][2].substr(0, program[i][2].size()-4));
            temp.rs1 = program[i][2].substr(program[i][2].size()-3, 2);
            instructions.push_back(temp);

        }else if(program[i][0] == "STORE") { //STORE x1, offset(x2) store from rA to memory address rB+offset
            instruction temp;               // Make sure to use rs1 and rs2 for this instruction
                                            // store rA to memory address rB+offset
                                            // rs1 is the register to store and rs2 is the register to add to the offset
            temp.type = program[i][0];
            temp.index = i;
            temp.cycle = 3;
            temp.rs1 = program[i][1];
            temp.offset = stoi(program[i][2].substr(0, program[i][2].size()-4));
            temp.rs2 = program[i][2].substr(program[i][2].size()-3, 2);
            instructions.push_back(temp);   

        }
        

    }

}

// print instructions with each field
void print_instructions(vector<instruction> instructions) {
    for(int i=0; i<instructions.size(); i++) {
        cout<<"---------------------------------"<<endl;
        cout<<"Type: "<<instructions[i].type<<endl;
        cout<<"Index: "<<instructions[i].index<<endl;
        cout<<"Cycle: "<<instructions[i].cycle<<endl;
        cout<<"Rd: "<<instructions[i].rd<<endl;
        cout<<"Rs1: "<<instructions[i].rs1<<endl;
        cout<<"Rs2: "<<instructions[i].rs2<<endl;
        cout<<"Imm: "<<instructions[i].imm<<endl;
        cout<<"Label: "<<instructions[i].label<<endl;
        cout<<"Offset: "<<instructions[i].offset<<endl;
        cout<<"isLabel: "<<instructions[i].isLabel<<endl;
        cout<<"---------------------------------"<<endl;
        cout<<endl;
    }
}

// get instructions
void get_instructions(string filename, vector<instruction> &instructions,map<string,int>&labels) {
    // read file
    vector<vector<string>> program;
    read_file(filename, program);
    // parse instructions
    parse_instructions(program, instructions);

    // get labels
    for(int i=0; i<instructions.size(); i++) {
        if(instructions[i].isLabel) {
            labels[instructions[i].label] = i;
        }
    }
    print_instructions(instructions);
    // print labels
    for(auto it=labels.begin(); it!=labels.end(); it++) {
        cout<<it->first<<" index: "<<it->second<<endl;
    }

}



int main() 
{
    cout<<"Starting"<<endl;
    vector<instruction> instructions;
    map<string, int> labels;

    get_instructions("instructions.txt", instructions,labels);
    
    return 0;
}