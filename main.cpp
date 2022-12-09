#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<vector>
#include<unordered_map>

using namespace std;

//global variables
unordered_map<string, int> regs; // 8 registers (r0-r7) IMPPP make sureeee r0 is always 0
unordered_map<int, int> mem; // 256 memory locations (m0-m255)




class RSRow {
    public:
    string name, instStatus;
    string type;
    int id, instIdx, remCycles;
    int result;
    bool busy, predicted;
    virtual void print() = 0;
    void decCounter() {
        remCycles--;
    }
    bool done() {
        return remCycles == 0;
    }
};

class LoadRSRow : public RSRow {
    public:
    int Vj, Qj, A;
    LoadRSRow(int idIn, string nameIn) {
        id = idIn;
        name = nameIn;
        busy = false;
        Vj = -1;
        Qj = -1;
        A = 0;
        predicted = false;
        type = "LOAD"
    }
    void free() {
        busy = false;
        Vj = -1;
        Qj = -1;
        A = 0;
        predicted = false;
    }
    void print() {
        cout << "id: " << id << '\n';
    }
};

class StoreRSRow : public RSRow {
    public:
    int Vj, Qj, Vk, Qk, A;
    StoreRSRow(int idIn, string nameIn) {
        id = idIn;
        name = nameIn;
        busy = false;
        Vj = -1;
        Qj = -1;
        Vk = -1;
        Qk = -1;
        A = 0;
        predicted = false;
        type = "STORE"
    }
    void free() {
        busy = false;
        Vj = -1;
        Qj = -1;
        Vk = -1;
        Qk = -1;
        A = 0;
        predicted = false;
    }
    void print() {
        cout << "id: " << id << '\n';
    }
};

class BEQRSRow : public RSRow {
    public:
    int Vj, Qj, Vk, Qk, pc, jumpPC;
    bool jump;
    BEQRSRow(int idIn, string nameIn) {
        id = idIn;
        name = nameIn;
        busy = false;
        Vj = -1;
        Qj = -1;
        Vk = -1;
        Qk = -1;
        predicted = false;
        type = "BEQ";
    }
    void free() {
        busy = false;
        Vj = -1;
        Qj = -1;
        Vk = -1;
        Qk = -1;
        predicted = false;
    }
    void print() {
        cout << "id: " << id << '\n';
    }
};


class UJumpRSRow : public RSRow {
    public:
    string op;
    int pc, jumpPC;
    bool jump;
    UJumpRSRow(int idIn, string nameIn) {
        id = idIn;
        name = nameIn;
        busy = false;
        predicted = false;
        type = "JAL";
    }
    void free() {
        busy = false;
        predicted = false;
    }
    void print() {
        cout << "id: " << id << '\n';
    }
};

class AdderRSRow : public RSRow {
    public:
    string op;
    int Vj, Qj, Vk, Qk;
    AdderRSRow(int idIn, string nameIn) {
        id = idIn;
        name = nameIn;
        busy = false;
        predicted = false;
        type = "ADD";
    }
    void free() {
        busy = false;
        predicted = false;
    }
    void print() {
        cout << "id: " << id << '\n';
    }
};

class NEGRSRow : public RSRow {
    public:
    int Vj, Qj;
    NEGRSRow(int idIn, string nameIn) {
        id = idIn;
        name = nameIn;
        Vj = -1;
        Qj = -1;
        busy = false;
        predicted = false;
        type = "NEG";
    }
    void free() {
        busy = false;
        Vj = -1;
        Qj = -1;
        predicted = false;
    }
    void print() {
        cout << "id: " << id << '\n';
    }
};

class NORRSRow : public RSRow {
    public:
    string op;
    int Vj, Qj, Vk, Qk;
    NORRSRow(int idIn, string nameIn) {
        id = idIn;
        name = nameIn;
        busy = false;
        predicted = false;
        type = "NOR";
    }
    void free() {
        busy = false;
        Vj = -1;
        Qj = -1;
        predicted = false;
    }
    void print() {
        cout << "id: " << id << '\n';
    }
};

class MULRSRow : public RSRow {
    public:
    string op;
    int Vj, Qj, Vk, Qk;
    MULRSRow(int idIn, string nameIn) {
        id = idIn;
        name = nameIn;
        busy = false;
        Vj = -1;
        Qj = -1;
        Vk = -1;
        Qk = -1;
        predicted = false;
        type = "MUL";
    }
    void free() {
        busy = false;
        Vj = -1;
        Qj = -1;
        Vk = -1;
        Qk = -1;
        predicted = false;
    }
    void print() {
        cout << "id: " << id << '\n';
    }
};



struct instruction {
    string type = ""; // instruction type
    int index = -1; // index in program
    int cycle = -1; // cycle numbers needed
    string rs1,rs2,rd = ""; // registers if exist
    int imm =0; // immediate if exist
    int offset = 0; // offset if exist
    string label_before = ""; // to not confuse labels with instructions in the beginning of the line
    string label_after = ""; // to not confuse labels with instructions in the end of the line

    
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
            temp.label_before = program[i][0].substr(0, program[i][0].size()-1);
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
            } else if(program[i][1]=="BEQ"){ //l1: BEQ rA, rB, label
                temp.type = program[i][1];
                temp.index = i;
                temp.cycle = 1;
                temp.rs1 = program[i][2];
                temp.rs2 = program[i][3];
                temp.label_after = program[i][4];
                instructions.push_back(temp);

            } else if(program[i][1] == "JAL"){ //L2: JAL label
                temp.type = program[i][1];
                temp.index = i;
                temp.cycle = 1;
                temp.label_after = program[i][2];
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
            temp.label_after = program[i][1];
            instructions.push_back(temp);


        }else if(program[i][0] == "RET") { //RET
            instruction temp;
            temp.type = program[i][0];
            temp.index = i;
            temp.cycle = 1;
            instructions.push_back(temp);

        }else if(program[i][0] == "BEQ") { //BEQ rA, rB, label
            instruction temp;
            temp.type = program[i][1];
            temp.index = i;
            temp.cycle = 1;
            temp.rs1 = program[i][2];
            temp.rs2 = program[i][3];
            temp.label_after = program[i][4];
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
        cout<<"Offset: "<<instructions[i].offset<<endl;
        cout<<"Label before: "<<instructions[i].label_before<<endl;
        cout<<"Label after: "<<instructions[i].label_after<<endl;
        cout<<"---------------------------------"<<endl;
        cout<<endl;
    }
}

// get instructions
void get_instructions(string filename, vector<instruction> &instructions,unordered_map<string,int>&labels) {
    // read file
    vector<vector<string>> program;
    read_file(filename, program);
    // parse instructions
    parse_instructions(program, instructions);

    // get labels
    for(int i=0; i<instructions.size(); i++) {
        if(instructions[i].label_before != "") {
            labels[instructions[i].label_before] = i;
        }
    }
    print_instructions(instructions);
    // print labels
    cout<<"-------------labels--------------"<<endl;
    for(auto it=labels.begin(); it!=labels.end(); it++) {
        cout<<it->first<<" index: "<<it->second<<endl;
    }
    cout<<"---------------------------------"<<endl;

}




void add(string src1, string src2, string des)
{
    regs[des] = regs[src1] + regs[src2];
}
void neg(string src1, string des)
{
    regs[des] = ~(regs[src1]);
}
void nor(string src1, string src2, string des)
{
    regs[des] = ~(regs[src1] | regs[src2]);
}
void addi(string src1, int imm, string des)
{
    regs[des] = regs[src1] + imm;
}
void mul(string src1, string src2, string des)
{
    regs[des] = regs[src1] * regs[src2];
}
bool beq(string src1, string src2)
{
    if (regs[src1] == regs[src2])
        return true;
    else
        return false;
}

void load(string ra, int off, string rb)
{
    regs[ra] = mem[off + regs[rb]];
}

void store(string ra, int off, string rb)
{
    mem[off + regs[rb]] = regs[ra];
}





int main() 
{



    cout<<"Starting"<<endl;

    vector<RSRow*> reservTable;

    LoadRSRow *load1 = new LoadRSRow(0, "load1");
    reservTable.push_back(load1);
    LoadRSRow *load2 = new LoadRSRow(1, "load2");
    reservTable.push_back(load2);

    StoreRSRow *store1 = new StoreRSRow(2, "store1");
    reservTable.push_back(store1);
    StoreRSRow *store2 = new StoreRSRow(3, "store2");
    reservTable.push_back(store2);

    BEQRSRow *beq1 = new BEQRSRow(4, "beq1");
    reservTable.push_back(beq1);

    UJumpRSRow *jump1 = new UJumpRSRow(5, "jump1");
    reservTable.push_back(jump1);

    AdderRSRow *adder1 = new AdderRSRow(6, "adder1");
    reservTable.push_back(adder1);
    AdderRSRow *adder2 = new AdderRSRow(7, "adder2");
    reservTable.push_back(adder2);
    AdderRSRow *adder3 = new AdderRSRow(8, "adder3");
    reservTable.push_back(adder3);

    NEGRSRow *neg1 = new NEGRSRow(9, "neg1");
    reservTable.push_back(neg1);

    NORRSRow *nor1 = new NORRSRow(10, "nor1");
    reservTable.push_back(nor1);

    MULRSRow *mul1 = new MULRSRow(11, "mul1");
    reservTable.push_back(mul1);

    vector<instruction> instructions;
    unordered_map<string, int> labels;
   
    for(int i=0; i<8; i++) {
        regs["r"+to_string(i)] = 0;
    }

    get_instructions("instructions.txt", instructions,labels);
    int pc = 0;
    while(true)
    {
        instruction current = instructions[pc];
        string type = current.type;
        string rs;


        // Handle having a station handling multiple instructions
        if (current.type == "ADDI")
            rs = "ADD";
        else if (current.type == "RET")
            rs = "JAL";
        else
            rs = current.type;

        for (int i = 0; i < reservTable.size(); i++)
        {
            if ((reservTable[i]->type == rs) && (!reservTable[i]->busy)){
                reservTable[i]->issue(current);
            }

        }
        
        


        



    }

    



    return 0;
}