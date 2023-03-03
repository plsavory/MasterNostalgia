/*
Mastalgia - a (soon to be) Sega Master System emulator.
Licensed under the GPLv3 license.
@author: Peter Savory
 */

#include "Z80IO.h"
#include "Utils.h"

#define DEBUG_VALUES

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
    subtractNegative = 1,
    overflowParity = 2,
    xf = 3,
    halfCarry = 4,
    yf = 5,
    zero = 6,
    sign = 7
};

enum ShiftBitToCopy {
    copyCarryFlag,
    copyPreviousValue,
    copyZero,
    copyNothing,
    copyOne
};

enum class ResetRequest : unsigned char {
    none = 0,
    pending = 1,
    processing = 2
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
    CPUZ80(Memory *smsMemory, Z80IO *z80Io);

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
    CPURegister originalRegisterValues[10];
    unsigned short originalStackPointerValue;
    CPUState state;
    unsigned char registerI;
    unsigned char registerR;
    std::string executedInstructionName = "";
    unsigned short displayOpcodePrefix;
    unsigned char displayOpcode;

    unsigned short readValue;

    unsigned char ioPortAddress;

    unsigned short memoryAddress;

    ResetRequest resetRequest;

    Z80IO *z80Io;

    // Interrupt flip flops
    bool iff1;
    bool iff2;
    bool enableInterrupts;
    bool bitUseMemory;

    void extendedOpcodes();

    void bitOpcodes();

    void indexOpcodes(unsigned char opcodePrefix, const std::string& indexPrefix, cpuReg indexRegister);

    void ixOpcodes();

    void iyOpcodes();

    void indexBitOpcodes(unsigned char opcodePrefix, const std::string& indexPrefix, cpuReg indexRegister);

    int executeOpcode();

    Memory *memory;
    int cyclesTaken;

    unsigned char NB();

    unsigned char NBHideFromTrace();

    signed char signedNB();

    unsigned char interruptMode; // TODO: Use http://z80.info/1653.htm as reference when implementing interrupts in future.

    void logCPUState();

    void jpCondition(JPCondition condition);

    void jrCondition(JPCondition condition, unsigned char offset);

    void retCondition(JPCondition condition);

    void jr(unsigned char offset);

    void jpImm();

    void djnz();

    // Instruction handler functions
    void ldReg8(unsigned char &dest, unsigned char value);

    void ldReg8(unsigned char &dest, unsigned char value, bool modifyFlags);

    void ldReg16(unsigned short &dest, unsigned short value, bool modifyFlags);

    void ldReg16(unsigned short &dest, unsigned short value);

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

    void callCondition(JPCondition condition);

    void rst(unsigned short location);

    void store(unsigned short location, unsigned char hi, unsigned char lo);

    unsigned char shiftLeft(unsigned char dest, ShiftBitToCopy copyMode);

    unsigned char rlc(unsigned char dest);

    unsigned char rl(unsigned char dest);

    unsigned char sla(unsigned char dest);

    unsigned char sll(unsigned char dest);

    unsigned char shiftRight(unsigned char dest, ShiftBitToCopy copyMode);

    unsigned char rrc(unsigned char dest);

    unsigned char rr(unsigned char dest);

    unsigned char sra(unsigned char dest);

    unsigned char srl(unsigned char dest);

    void rld(unsigned char &dest);

    void rrd(unsigned char &dest);

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

    bool hasMetJumpCondition(JPCondition condition);

    void readPortToRegister(unsigned char &dest, unsigned char portAddress);

    void ini(bool increment);

    void inir(bool increment);

    void cpi(bool increment);

    void cpir(bool increment);

    void ldi(bool increment);

    void ldir(bool increment);

    void outi(bool increment);

    void otir(bool increment);

    void retn();

    void reti();

    void bit(unsigned char bitNumber, unsigned char value);

    unsigned char res(unsigned char bitNumber, unsigned char value);

    unsigned char set(unsigned char bitNumber, unsigned char value);

    // Stack
    void pushStack(unsigned char value);

    void pushStack(unsigned short value);

    unsigned char popStack();

    unsigned short popStack16();

    void exStack(unsigned short &dest);

    unsigned short getIndexedOffsetAddress(unsigned short registerValue);

    inline void handleUndocumentedFlags(unsigned char result) {
        setFlag(CPUFlag::yf, Utils::testBit(5, result));
        setFlag(CPUFlag::xf, Utils::testBit(3, result));
    }

    inline void handleUndocumentedFlags(unsigned short result) {
        setFlag(CPUFlag::yf, Utils::testBit(5, result));
        setFlag(CPUFlag::xf, Utils::testBit(3, result));
    }

    // Misc
    std::string getInstructionName(unsigned short opcode, unsigned short extendedOpcode, unsigned short lastOpcode);

    std::vector<std::string> getStandardInstructionNames();

    std::vector<std::string> getBitInstructionNames();

    std::vector<std::string> getIXInstructionNames();

    std::vector<std::string> getIXBitInstructionNames();

    std::vector<std::string> getMiscInstructionNames();

    std::vector<std::string> getIYInstructionNames();

    std::vector<std::string> getIYBitInstructionNames();

    void portOut(unsigned char port, unsigned char value);

    unsigned char portIn(unsigned char port);

    void writeMemory(unsigned short location, unsigned char value);

    void writeMemory(unsigned short location, unsigned short value);

    unsigned char readMemory(unsigned short location);

    unsigned short readMemory16Bit(unsigned short location);

    inline bool getHalfCarry(unsigned char originalValue, unsigned char newValue) {
        return (originalValue ^ newValue ^ 1) & 0x10;
    }

    inline bool getParity(unsigned char value) {
        return std::bitset<8>(value).count() % 2 == 0;
    }
};
