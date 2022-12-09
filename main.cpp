#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<vector>

using namespace std;


struct instruction {
    string type = ""; // instruction type
    int index = -1; // index in program
    int cycle = -1; // cycle numbers needed
    string label = ""; // empty string if no label
    string rs1,rs2,rd = ""; // registers if exist
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

        //check if ADD, NOR, MUL
        // no label
        if(program[i][0] == "ADD" || program[i][0] == "NOR" || program[i][0] == "MUL") {
            instruction temp;
            temp.type = program[i][0];
            temp.index = i;
            temp.cycle = 1; // check in project description
            temp.rd = program[i][1];
            temp.rs1 = program[i][2];
            temp.rs2 = program[i][3];
            instructions.push_back(temp);
        }
    }



}


int main() 
{
    vector<vector<string>> program;
    cout<<"Starting"<<endl;
    read_file("instructions.txt", program);
    vector<instruction> instructions;
    // print vector
    // for(int i=0; i<program.size(); i++) {
    //     for(int j=0; j<program[i].size(); j++) {
    //         cout<<program[i][j]<<" ";
    //     }
    //     cout<<endl;
    // }

    parse_instructions(program, instructions);

    //print instructions with each field
    for(int i=0; i<instructions.size(); i++) {
        cout<<"Type: "<<instructions[i].type<<endl;
        cout<<"Index: "<<instructions[i].index<<endl;
        cout<<"Cycle: "<<instructions[i].cycle<<endl;
        cout<<"Label: "<<instructions[i].label<<endl;
        cout<<"RS1: "<<instructions[i].rs1<<endl;
        cout<<"RS2: "<<instructions[i].rs2<<endl;
        cout<<"RD: "<<instructions[i].rd<<endl;
        cout<<endl;
    }
    
    return 0;
}