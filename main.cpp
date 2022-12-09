#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<vector>
#include<unordered_map>

using namespace std;

//global variables for memory and reg 
unordered_map<string, int> regs; // 8 registers (r0-r7) IMPPP make sureeee r0 is always 0
unordered_map<int, int> mem; // 256 memory locations (m0-m255)
unordered_map<string, int> labels; // labels found 
unordered_map<string, int> reg_status; // register status

int pc = 0; // program counter

int neg(int src1)
{
    return ~src1;
}
int nor(int src1, int src2)
{
    return(~(src1 | src2));
}
int add(int src1, int src2)
{
    return src1+ src2;
}
int addi(int src1, int imm)
{
    return src1 + imm;
}
int mul(int src1, int src2)
{
    return src1 * src2;
}
bool beq(int src1, int src2){   
    return src1 == src2;
}
int load(int address)
{
    return mem[address];
}

struct instruction {
    string type = ""; // instruction type
    int index = -1; // index in program
    int cycle = -1; // cycle numbers needed
    string rs1, rs2, rd = ""; // registers if exist
    int imm = 0; // immediate if exist
    int offset = 0; // offset if exist
    string label_before = ""; // to not confuse labels with instructions in the beginning of the line
    string label_after = ""; // to not confuse labels with instructions in the end of the line
};

class RSRow {
    public:
    string name, instStatus;
    string type;
    int id, instIdx, remCycles;
    int result;
    bool busy, predicted;
    virtual void print() = 0;
    virtual void issue(instruction inst) = 0;
    virtual void execute() = 0;
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
        type = "LOAD";
    }
    void free() {
        busy = false;
        Vj = -1;
        Qj = -1;
        A = 0;
        predicted = false;
    }
    void issue(instruction inst) {
        instIdx = inst.index;
        remCycles = inst.cycle;
        A = inst.offset;
        if (reg_status[inst.rs1] != -1) {
            Qj = reg_status[inst.rs1];
        } else {
            Vj = regs[inst.rs1];
            Qj = 0;
        }
        busy = true;
        reg_status[inst.rd] = id;
    }
    void execute( ) {
        A = A+Vj;
        result =load(A);
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
        type = "STORE";
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
    void issue(instruction inst) {
        instIdx = inst.index;
        remCycles = inst.cycle;
        A = inst.offset;
        if (reg_status[inst.rs1] != -1) {
            Qj = reg_status[inst.rs1];
        } else {
            Vj = regs[inst.rs1];
            Qj = 0;
        }
        if (reg_status[inst.rs2] != -1) {
            Qk = reg_status[inst.rs2];
        } else {
            Vk = regs[inst.rs2];
            Qk = 0;
        }
        busy = true;
        reg_status[inst.rd] = id;
    }
    void execute( ) {
        A = A + Vj;
    }
    void print() {
        cout << "id: " << id << '\n';
    }
};

class BEQRSRow : public RSRow {
    public:
    int Vj, Qj, Vk, Qk, currPC, jumpPC;
    string label;
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
    void issue(instruction inst) {
        instIdx = inst.index;
        remCycles = inst.cycle;
        currPC = pc;
        if (reg_status[inst.rs1] != -1) {
            Qj = reg_status[inst.rs1];
        } else {
            Vj = regs[inst.rs1];
            Qj = 0;
        }
        if (reg_status[inst.rs2] != -1) {
            Qk = reg_status[inst.rs2];
        } else {
            Vk = regs[inst.rs2];
            Qk = 0;
        }
        jumpPC = labels[inst.label_after];
        busy = true;
    }
        void execute() {
       jump =beq(Vj,Qj);
       jumpPC=labels[label];
    }
    void print() {
        cout << "id: " << id << '\n';
    }
};

class UJumpRSRow : public RSRow {
    public:
    string op;
    int currPC, jumpPC, Vj, Qj;
    string label;
    bool jump;
    UJumpRSRow(int idIn, string nameIn) {
        id = idIn;
        name = nameIn;
        busy = false;
        predicted = false;
        type = "JAL";
        Vj = -1;
        Qj = -1;
        label = "";
    }
    void free() {
        busy = false;
        predicted = false;
        Vj = -1;
        Qj = -1;
        label = "";
    }
    void issue(instruction inst) {
        instIdx = inst.index;
        remCycles = inst.cycle;
        busy = true;
        op = inst.type;
        if (op == "JAL") {
            reg_status["r1"] = id;
            currPC = pc;
            jumpPC = labels[inst.label_after];
        } else {
            if (reg_status["r1"] != -1) {
                Qj = reg_status["r1"];
            } else {
                Vj = regs["r1"];
                Qj = 0;
            }
        }
    }
    void execute() {
       jump =1;
       jumpPC=labels[label];
    }
    void print() {
        cout << "id: " << id << '\n';
    }
};

class AdderRSRow : public RSRow {
    public:
    string op;
    int Vj, Qj, Vk, Qk, imm;
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
    void issue(instruction inst) {
        instIdx = inst.index;
        remCycles = inst.cycle;
        op = inst.type;
        if (reg_status[inst.rs1] != -1) {
            Qj = reg_status[inst.rs1];
        } else {
            Vj = regs[inst.rs1];
            Qj = 0;
        }
        if (op == "ADD") {
            if (reg_status[inst.rs2] != -1) {
                Qk = reg_status[inst.rs2];
            } else {
                Vk = regs[inst.rs2];
                Qk = 0;
            }
        } else {
            imm = inst.imm;
        }
        busy = true;
        reg_status[inst.rd] = id;
    }
    void execute() {
        if (op=="ADDI")
        result=addi(Vj,Vk);
        else
        result =add(Vj,Vk);
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
    void issue(instruction inst) {
        instIdx = inst.index;
        remCycles = inst.cycle;
        if (reg_status[inst.rs1] != -1) {
            Qj = reg_status[inst.rs1];
        } else {
            Vj = regs[inst.rs1];
            Qj = 0;
        }
        busy = true;
        reg_status[inst.rd] = id;
    }
    void execute() {
        result = neg(Vj);
    }
    void print() {
        cout << "id: " << id << '\n';
    }
};

class NORRSRow : public RSRow {
    public:
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
        Vk = -1;
        Qk = -1;
        predicted = false;
    }
    void issue(instruction inst) {
        instIdx = inst.index;
        remCycles = inst.cycle;
        if (reg_status[inst.rs1] != -1) {
            Qj = reg_status[inst.rs1];
        } else {
            Vj = regs[inst.rs1];
            Qj = 0;
        }
        if (reg_status[inst.rs2] != -1) {
            Qk = reg_status[inst.rs2];
        } else {
            Vk = regs[inst.rs2];
            Qk = 0;
        }
        busy = true;
        reg_status[inst.rd] = id;
    }
    void execute() {
        result=nor(Vj,Vk);
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
    void issue(instruction inst) {
        instIdx = inst.index;
        remCycles = inst.cycle;
        if (reg_status[inst.rs1] != -1) {
            Qj = reg_status[inst.rs1];
        } else {
            Vj = regs[inst.rs1];
            Qj = 0;
        }
        if (reg_status[inst.rs2] != -1) {
            Qk = reg_status[inst.rs2];
        } else {
            Vk = regs[inst.rs2];
            Qk = 0;
        }
        busy = true;
        reg_status[inst.rd] = id;
    }
    void execute( ) {
        result=mul(Vj,Vk);
    }
    void print() {
        cout << "id: " << id << '\n';
    }
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
            temp.cycle = 2;
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
    
    for(int i=0; i<8; i++) {
        regs["r"+to_string(i)] = 0;
    }


    // reg_status contains registers from r0 to r7 and their status initially set to -1
    for(int i=0; i<8; i++) {
        reg_status["r"+to_string(i)] = -1;
    }


    get_instructions("instructions.txt", instructions,labels);
    int cycles = 0;
    int instruction_counter = 0;
    while(true)
    {
        cycles++;
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


        // execute instructions
        // for(int i = 0; i<reservTable.size(); i++) {
        //     if(reservTable[i]->type == "ADD" || reservTable[i]->type == "ADDI" || reservTable[i]->type == "NOR" || reservTable[i]->type == "MUL" || reservTable[i]->type=="BEQ"){
               

        //     }

        // }


    }
    return 0;
}