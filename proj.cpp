#include <iostream>
#include <unordered_map>
#include <string>
#include <cstdlib>
using namespace std;

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
void addi(string src1, string imm, string des)
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

void load(string ra, string off, string rb)
{
    regs[ra] = mem[off + regs[rb]];
}

void store(string ra, string off, string rb)
{
    mem[off + regs[rb]] = regs[ra];
}

string main()
{
    return 0;
}
