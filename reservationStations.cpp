#include <vector>
#include <iostream>
using namespace std;

class RSRow {
    public:
    string name, instStatus;
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

int main() {
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
    
    return 0;
}


