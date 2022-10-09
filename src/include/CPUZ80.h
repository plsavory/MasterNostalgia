/*
Mastalgia - a (soon to be) Sega Master System emulator.
Licensed under the GPLv3 license.
@author: Peter Savory
 */

enum CPUState {
    Halt, Running, Error, Step
};

// For a nice way to address the CPU registers in the code...
enum cpuReg {
    AF, BC, DE, HL, AFS, BCS, DES, HLS, IX, IY
};

// Used for conditional jump operations (Following info from: http://www.z80.info/z80code.htm)
// NZ        Jump if zero flag = 0. (last Z flag instr <>0)
// Z         Jump if zero flag = 1. (last Z flag instr = 0)
// NC        Jump if carry flag = 0. (last C instr = no carry)
// C         Jump if carry flag = 1. (last C instr = carry)
// PO        Jump if parity odd, parity flag = 0.
// PE        Jump if parity even, parity flag = 1.
// P         Jump if sign positive, sign flag = 0.
// M         Jump if sign negative (minus), sign flag = 1.
enum JPCondition {
    NZ, Z, NC, C, PO, PE, P, M
};

// TODO: Ensure that the bits are actually in the correct orientation with testing
enum CPUFlag : unsigned char {
    carry = 0,
    subtract = 1,
    overflow = 2,
    unused2 = 3,
    halfCarry = 4,
    unused = 5,
    zero = 6,
    sign = 7
};

// Force these variables to use the same memory space - a handy way of emulating the CPU registers.
union CPURegister {
    unsigned short whole;

    struct {
        unsigned char lo;
        unsigned char hi;
    };
};

class CPUZ80 {
public:
    CPUZ80(Memory *smsMemory);

    ~CPUZ80();

    void reset();

    int execute();

    CPUState getState() {
        return this->state;
    }

private:
    unsigned short programCounter;
    unsigned short originalProgramCounterValue;
    unsigned short stackPointer;
    CPURegister gpRegisters[10];
    CPUState state;
    unsigned short registerI;
    unsigned short RegisterR;
    std::string executedInstructionName = "";

    // Interrupt flip flops
    bool iff1;
    bool iff2;
    bool enableInterrupts;

    void extendedOpcodes(unsigned char opcode);

    void iyOpcodes(unsigned char opcode);

    int executeOpcode();

    Memory *memory;
    int cyclesTaken;

    unsigned char NB();

    signed char signedNB();

    unsigned char interruptMode; // TODO: Use http://z80.info/1653.htm as reference when implementing interrupts in future.

    void logCPUState(unsigned char opcode, std::string prefix);

    void jpCondition(JPCondition condition);

    void jrCondition(JPCondition condition, unsigned char offset);

    void jr(unsigned char offset);

    void jpImm();

    void djnz(unsigned short from, unsigned char offset);

    // Instruction handler functions
    void ldReg8(unsigned char &dest, unsigned char value);

    void ldReg16(unsigned short &dest, unsigned short value, bool modifyFlags);

    void addAdc8Bit(unsigned char &dest, unsigned char value, bool withCarry);

    void add8Bit(unsigned char &dest, unsigned char value);

    void adc8Bit(unsigned char &dest, unsigned char value);

    void addAdc16Bit(unsigned short &dest, unsigned short value, bool withCarry);

    void add16Bit(unsigned short &dest, unsigned short value);

    void adc16Bit(unsigned short &dest, unsigned short value);

    void subSbc8Bit(unsigned char &dest, unsigned char value, bool withCarry);

    void sub8Bit(unsigned char &dest, unsigned char value);

    void sbc8Bit(unsigned char &dest, unsigned char value);

    void sbc16Bit(unsigned short &dest, unsigned short value);

    void and8Bit(unsigned char &dest, unsigned char value);

    void or8Bit(unsigned char &dest, unsigned char value);

    void setInterruptMode(unsigned char mode);

    void exclusiveOr(unsigned char &dest, unsigned char value);

    void inc16Bit(unsigned short &target);

    void inc8Bit(unsigned char &target);

    unsigned char getInc8BitValue(unsigned char initialValue);

    void dec8Bit(unsigned char &dest);

    unsigned char getDec8BitValue(unsigned char initialValue);

    void dec16Bit(unsigned short &target);

    void compare8Bit(unsigned char valueToSubtract);

    void call(unsigned short location);

    void call(unsigned short location, bool conditionMet);

    void store(unsigned short location, unsigned char hi, unsigned char lo);

    void shiftLeft(unsigned char &dest, bool copyPreviousCarryFlagValue);

    void rlc(unsigned char &dest);

    void rl(unsigned char &dest);

    void shiftRight(unsigned char &dest, bool copyPreviousCarryFlagValue);

    void rrc(unsigned char &dest);

    void rr(unsigned char &dest);

    void cpl(unsigned char &dest);

    void exchange8Bit(unsigned char &register1, unsigned char &register2);

    void exchange16Bit(unsigned short &register1, unsigned short &register2);

    void popStackExchange(unsigned short &destinationRegister);

    void da(unsigned char &dest);

    // To make flag handling easier and to prevent repetitive typing
    void setFlag(CPUFlag flag, bool value);

    bool getFlag(CPUFlag flag);

    // Memory management
    unsigned short build16BitNumber();

    unsigned char getIndirectValue();

    unsigned char getIndirectValue(unsigned short address);

    // IO
    void writeIOPort(unsigned char address, unsigned char value);

    unsigned char readIOPort(unsigned char address);

    bool hasMetJumpCondition(JPCondition condition);

    void ldir();

    void otir();

    // Stack
    void pushStack(unsigned char value);

    void pushStack(unsigned short value);

    unsigned char popStack();

    unsigned short popStack16();

    // Misc
    std::string getInstructionName(unsigned short opcode, unsigned short extendedOpcode, unsigned short lastOpcode);

    std::vector<std::string> getStandardInstructionNames();

    std::vector<std::string> getBitInstructionNames();

    std::vector<std::string> getIXInstructionNames();

    std::vector<std::string> getIXBitInstructionNames();

    std::vector<std::string> getMiscInstructionNames();

    std::vector<std::string> getIYInstructionNames();

    std::vector<std::string> getIYBitInstructionNames();

};
