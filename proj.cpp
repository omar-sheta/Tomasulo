#include <iostream>
#include <unordered_map>
#include <string>
#include <cstdlib>
using namespace std;

unordered_map<int, int> mem;
unordered_map<int, int> regs;

void add(int src1, int src2, int des)
{
    regs[des] = regs[src1] + regs[src2];
}
void neg(int src1, int des)
{
    regs[des] = ~(regs[src1]);
}
void nor(int src1, int src2, int des)
{
    regs[des] = ~(regs[src1] | regs[src2]);
}
void addi(int src1, int imm, int des)
{
    regs[des] = regs[src1] + imm;
}
void mul(int src1, int src2, int des)
{
    regs[des] = regs[src1] * regs[src2];
}
bool beq(int src1, int src2)
{
    if (regs[src1] == regs[src2])
        return true;
    else
        return false;
}

void load(int ra, int off, int rb)
{
    regs[ra] = mem[off + regs[rb]];
}

void store(int ra, int off, int rb)
{
    mem[off + regs[rb]] = regs[ra];
}

int main()
{
    return 0;
}
