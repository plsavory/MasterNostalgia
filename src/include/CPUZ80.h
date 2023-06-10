/*
Mastalgia - a (soon to be) Sega Master System emulator.
Licensed under the GPLv3 license.
@author: Peter Savory
 */

#include <bitset>
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
    copyOutgoingValue,
    copyZero,
    preserve,
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
    typedef void (CPUZ80::*OpcodeHandler) ();

    OpcodeHandler standardOpcodeHandlers[256]{};
    OpcodeHandler extendedOpcodeHandlers[256]{};
    OpcodeHandler bitOpcodeHandlers[256]{};
    OpcodeHandler indexOpcodeHandlers[256]{};
    OpcodeHandler indexBitOpcodeHandlers[256]{};

    cpuReg indexRegisterForCurrentOpcode;
    unsigned short indexedAddressForCurrentOpcode{};

    unsigned short programCounter{};
    unsigned short originalProgramCounterValue{};
    unsigned short stackPointer{};
    CPURegister gpRegisters[10]{};
    CPURegister originalRegisterValues[10]{};
    unsigned short originalStackPointerValue{};
    CPUState state;
    unsigned char registerI{};
    unsigned char registerR{};
    std::string executedInstructionName = "";
    unsigned short displayOpcodePrefix{};
    unsigned char displayOpcode{};

    unsigned short readValue{};

    unsigned char ioPortAddress{};

    unsigned short memoryAddress{};

    ResetRequest resetRequest;

    Z80IO *z80Io;

    // Interrupt flip flops
    bool iff1{};
    bool iff2{};
    bool enableInterrupts{};
    bool bitUseMemory{};

    void initialiseOpcodeHandlerPointers();

    void extendedOpcodes();

    void bitOpcodes();

    void indexOpcodes(cpuReg indexRegister);

    void indexBitOpcodes(cpuReg indexRegister);

    int executeOpcode();

    Memory *memory;
    int cyclesTaken;

    unsigned char NB();

    unsigned char NBHideFromTrace();

    signed char signedNB();

    unsigned char interruptMode; // TODO: Use http://z80.info/1653.htm as reference when implementing interrupts in future.

    void logCPUState();

    void jpCondition(JPCondition condition);

    void jrCondition(JPCondition condition);

    void retCondition(JPCondition condition);

    void jr();

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

    void subSbc16Bit(unsigned short &dest, unsigned short value, bool withCarry);

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

    unsigned char shiftLeft(unsigned char dest, ShiftBitToCopy lowBitCopyMode, bool updateAllFlags);

    unsigned char rlc(unsigned char dest);

    void rlca();

    unsigned char rl(unsigned char dest);

    void rla();

    unsigned char sla(unsigned char dest);

    unsigned char sll(unsigned char dest);

    unsigned char shiftRight(unsigned char dest, ShiftBitToCopy highBitCopyMode, bool updateAllFlags);

    unsigned char rrc(unsigned char dest);

    void rrca();

    unsigned char rr(unsigned char dest);

   void rra();

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

    void neg();

    void bit(unsigned char bitNumber, unsigned char value);

    unsigned char res(unsigned char bitNumber, unsigned char value);

    unsigned char set(unsigned char bitNumber, unsigned char value);

    void ccf();

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

    // Opcode handlers
    void opcodeHandlerInvalid();
    void standardOpcodeHandler0x00();
    void standardOpcodeHandler0x01();
    void standardOpcodeHandler0x02();
    void standardOpcodeHandler0x03();
    void standardOpcodeHandler0x04();
    void standardOpcodeHandler0x05();
    void standardOpcodeHandler0x06();
    void standardOpcodeHandler0x07();
    void standardOpcodeHandler0x08();
    void standardOpcodeHandler0x09();
    void standardOpcodeHandler0x0A();
    void standardOpcodeHandler0x0B();
    void standardOpcodeHandler0x0C();
    void standardOpcodeHandler0x0D();
    void standardOpcodeHandler0x0E();
    void standardOpcodeHandler0x0F();
    void standardOpcodeHandler0x10();
    void standardOpcodeHandler0x11();
    void standardOpcodeHandler0x12();
    void standardOpcodeHandler0x13();
    void standardOpcodeHandler0x14();
    void standardOpcodeHandler0x15();
    void standardOpcodeHandler0x16();
    void standardOpcodeHandler0x17();
    void standardOpcodeHandler0x18();
    void standardOpcodeHandler0x19();
    void standardOpcodeHandler0x1A();
    void standardOpcodeHandler0x1B();
    void standardOpcodeHandler0x1C();
    void standardOpcodeHandler0x1D();
    void standardOpcodeHandler0x1E();
    void standardOpcodeHandler0x1F();
    void standardOpcodeHandler0x20();
    void standardOpcodeHandler0x21();
    void standardOpcodeHandler0x22();
    void standardOpcodeHandler0x23();
    void standardOpcodeHandler0x24();
    void standardOpcodeHandler0x25();
    void standardOpcodeHandler0x26();
    void standardOpcodeHandler0x27();
    void standardOpcodeHandler0x28();
    void standardOpcodeHandler0x29();
    void standardOpcodeHandler0x2A();
    void standardOpcodeHandler0x2B();
    void standardOpcodeHandler0x2C();
    void standardOpcodeHandler0x2D();
    void standardOpcodeHandler0x2E();
    void standardOpcodeHandler0x2F();
    void standardOpcodeHandler0x30();
    void standardOpcodeHandler0x31();
    void standardOpcodeHandler0x32();
    void standardOpcodeHandler0x33();
    void standardOpcodeHandler0x34();
    void standardOpcodeHandler0x35();
    void standardOpcodeHandler0x36();
    void standardOpcodeHandler0x37();
    void standardOpcodeHandler0x38();
    void standardOpcodeHandler0x39();
    void standardOpcodeHandler0x3A();
    void standardOpcodeHandler0x3B();
    void standardOpcodeHandler0x3C();
    void standardOpcodeHandler0x3D();
    void standardOpcodeHandler0x3E();
    void standardOpcodeHandler0x3F();
    void standardOpcodeHandler0x40();
    void standardOpcodeHandler0x41();
    void standardOpcodeHandler0x42();
    void standardOpcodeHandler0x43();
    void standardOpcodeHandler0x44();
    void standardOpcodeHandler0x45();
    void standardOpcodeHandler0x46();
    void standardOpcodeHandler0x47();
    void standardOpcodeHandler0x48();
    void standardOpcodeHandler0x49();
    void standardOpcodeHandler0x4A();
    void standardOpcodeHandler0x4B();
    void standardOpcodeHandler0x4C();
    void standardOpcodeHandler0x4D();
    void standardOpcodeHandler0x4E();
    void standardOpcodeHandler0x4F();
    void standardOpcodeHandler0x50();
    void standardOpcodeHandler0x51();
    void standardOpcodeHandler0x52();
    void standardOpcodeHandler0x53();
    void standardOpcodeHandler0x54();
    void standardOpcodeHandler0x55();
    void standardOpcodeHandler0x56();
    void standardOpcodeHandler0x57();
    void standardOpcodeHandler0x58();
    void standardOpcodeHandler0x59();
    void standardOpcodeHandler0x5A();
    void standardOpcodeHandler0x5B();
    void standardOpcodeHandler0x5C();
    void standardOpcodeHandler0x5D();
    void standardOpcodeHandler0x5E();
    void standardOpcodeHandler0x5F();
    void standardOpcodeHandler0x60();
    void standardOpcodeHandler0x61();
    void standardOpcodeHandler0x62();
    void standardOpcodeHandler0x63();
    void standardOpcodeHandler0x64();
    void standardOpcodeHandler0x65();
    void standardOpcodeHandler0x66();
    void standardOpcodeHandler0x67();
    void standardOpcodeHandler0x68();
    void standardOpcodeHandler0x69();
    void standardOpcodeHandler0x6A();
    void standardOpcodeHandler0x6B();
    void standardOpcodeHandler0x6C();
    void standardOpcodeHandler0x6D();
    void standardOpcodeHandler0x6E();
    void standardOpcodeHandler0x6F();
    void standardOpcodeHandler0x70();
    void standardOpcodeHandler0x71();
    void standardOpcodeHandler0x72();
    void standardOpcodeHandler0x73();
    void standardOpcodeHandler0x74();
    void standardOpcodeHandler0x75();
    void standardOpcodeHandler0x76();
    void standardOpcodeHandler0x77();
    void standardOpcodeHandler0x78();
    void standardOpcodeHandler0x79();
    void standardOpcodeHandler0x7A();
    void standardOpcodeHandler0x7B();
    void standardOpcodeHandler0x7C();
    void standardOpcodeHandler0x7D();
    void standardOpcodeHandler0x7E();
    void standardOpcodeHandler0x7F();
    void standardOpcodeHandler0x80();
    void standardOpcodeHandler0x81();
    void standardOpcodeHandler0x82();
    void standardOpcodeHandler0x83();
    void standardOpcodeHandler0x84();
    void standardOpcodeHandler0x85();
    void standardOpcodeHandler0x86();
    void standardOpcodeHandler0x87();
    void standardOpcodeHandler0x88();
    void standardOpcodeHandler0x89();
    void standardOpcodeHandler0x8A();
    void standardOpcodeHandler0x8B();
    void standardOpcodeHandler0x8C();
    void standardOpcodeHandler0x8D();
    void standardOpcodeHandler0x8E();
    void standardOpcodeHandler0x8F();
    void standardOpcodeHandler0x90();
    void standardOpcodeHandler0x91();
    void standardOpcodeHandler0x92();
    void standardOpcodeHandler0x93();
    void standardOpcodeHandler0x94();
    void standardOpcodeHandler0x95();
    void standardOpcodeHandler0x96();
    void standardOpcodeHandler0x97();
    void standardOpcodeHandler0x98();
    void standardOpcodeHandler0x99();
    void standardOpcodeHandler0x9A();
    void standardOpcodeHandler0x9B();
    void standardOpcodeHandler0x9C();
    void standardOpcodeHandler0x9D();
    void standardOpcodeHandler0x9E();
    void standardOpcodeHandler0x9F();
    void standardOpcodeHandler0xA0();
    void standardOpcodeHandler0xA1();
    void standardOpcodeHandler0xA2();
    void standardOpcodeHandler0xA3();
    void standardOpcodeHandler0xA4();
    void standardOpcodeHandler0xA5();
    void standardOpcodeHandler0xA6();
    void standardOpcodeHandler0xA7();
    void standardOpcodeHandler0xA8();
    void standardOpcodeHandler0xA9();
    void standardOpcodeHandler0xAA();
    void standardOpcodeHandler0xAB();
    void standardOpcodeHandler0xAC();
    void standardOpcodeHandler0xAD();
    void standardOpcodeHandler0xAE();
    void standardOpcodeHandler0xAF();
    void standardOpcodeHandler0xB0();
    void standardOpcodeHandler0xB1();
    void standardOpcodeHandler0xB2();
    void standardOpcodeHandler0xB3();
    void standardOpcodeHandler0xB4();
    void standardOpcodeHandler0xB5();
    void standardOpcodeHandler0xB6();
    void standardOpcodeHandler0xB7();
    void standardOpcodeHandler0xB8();
    void standardOpcodeHandler0xB9();
    void standardOpcodeHandler0xBA();
    void standardOpcodeHandler0xBB();
    void standardOpcodeHandler0xBC();
    void standardOpcodeHandler0xBD();
    void standardOpcodeHandler0xBE();
    void standardOpcodeHandler0xBF();
    void standardOpcodeHandler0xC0();
    void standardOpcodeHandler0xC1();
    void standardOpcodeHandler0xC2();
    void standardOpcodeHandler0xC3();
    void standardOpcodeHandler0xC4();
    void standardOpcodeHandler0xC5();
    void standardOpcodeHandler0xC6();
    void standardOpcodeHandler0xC7();
    void standardOpcodeHandler0xC8();
    void standardOpcodeHandler0xC9();
    void standardOpcodeHandler0xCA();
    void standardOpcodeHandler0xCB();
    void standardOpcodeHandler0xCC();
    void standardOpcodeHandler0xCD();
    void standardOpcodeHandler0xCE();
    void standardOpcodeHandler0xCF();
    void standardOpcodeHandler0xD0();
    void standardOpcodeHandler0xD1();
    void standardOpcodeHandler0xD2();
    void standardOpcodeHandler0xD3();
    void standardOpcodeHandler0xD4();
    void standardOpcodeHandler0xD5();
    void standardOpcodeHandler0xD6();
    void standardOpcodeHandler0xD7();
    void standardOpcodeHandler0xD8();
    void standardOpcodeHandler0xD9();
    void standardOpcodeHandler0xDA();
    void standardOpcodeHandler0xDB();
    void standardOpcodeHandler0xDC();
    void standardOpcodeHandler0xDD();
    void standardOpcodeHandler0xDE();
    void standardOpcodeHandler0xDF();
    void standardOpcodeHandler0xE0();
    void standardOpcodeHandler0xE1();
    void standardOpcodeHandler0xE2();
    void standardOpcodeHandler0xE3();
    void standardOpcodeHandler0xE4();
    void standardOpcodeHandler0xE5();
    void standardOpcodeHandler0xE6();
    void standardOpcodeHandler0xE7();
    void standardOpcodeHandler0xE8();
    void standardOpcodeHandler0xE9();
    void standardOpcodeHandler0xEA();
    void standardOpcodeHandler0xEB();
    void standardOpcodeHandler0xEC();
    void standardOpcodeHandler0xED();
    void standardOpcodeHandler0xEE();
    void standardOpcodeHandler0xEF();
    void standardOpcodeHandler0xF0();
    void standardOpcodeHandler0xF1();
    void standardOpcodeHandler0xF2();
    void standardOpcodeHandler0xF3();
    void standardOpcodeHandler0xF4();
    void standardOpcodeHandler0xF5();
    void standardOpcodeHandler0xF6();
    void standardOpcodeHandler0xF7();
    void standardOpcodeHandler0xF8();
    void standardOpcodeHandler0xF9();
    void standardOpcodeHandler0xFA();
    void standardOpcodeHandler0xFB();
    void standardOpcodeHandler0xFC();
    void standardOpcodeHandler0xFD();
    void standardOpcodeHandler0xFE();
    void standardOpcodeHandler0xFF();

    void extendedOpcodeHandler0x40();
    void extendedOpcodeHandler0x41();
    void extendedOpcodeHandler0x42();
    void extendedOpcodeHandler0x43();
    void extendedOpcodeHandler0x44();
    void extendedOpcodeHandler0x45();
    void extendedOpcodeHandler0x46();
    void extendedOpcodeHandler0x47();
    void extendedOpcodeHandler0x48();
    void extendedOpcodeHandler0x49();
    void extendedOpcodeHandler0x4A();
    void extendedOpcodeHandler0x4B();
    void extendedOpcodeHandler0x4D();
    void extendedOpcodeHandler0x4F();
    void extendedOpcodeHandler0x50();
    void extendedOpcodeHandler0x51();
    void extendedOpcodeHandler0x52();
    void extendedOpcodeHandler0x53();
    void extendedOpcodeHandler0x56();
    void extendedOpcodeHandler0x57();
    void extendedOpcodeHandler0x58();
    void extendedOpcodeHandler0x59();
    void extendedOpcodeHandler0x5A();
    void extendedOpcodeHandler0x5B();
    void extendedOpcodeHandler0x5E();
    void extendedOpcodeHandler0x5F();
    void extendedOpcodeHandler0x60();
    void extendedOpcodeHandler0x61();
    void extendedOpcodeHandler0x62();
    void extendedOpcodeHandler0x63();
    void extendedOpcodeHandler0x67();
    void extendedOpcodeHandler0x68();
    void extendedOpcodeHandler0x69();
    void extendedOpcodeHandler0x6A();
    void extendedOpcodeHandler0x6B();
    void extendedOpcodeHandler0x6F();
    void extendedOpcodeHandler0x70();
    void extendedOpcodeHandler0x71();
    void extendedOpcodeHandler0x72();
    void extendedOpcodeHandler0x73();
    void extendedOpcodeHandler0x78();
    void extendedOpcodeHandler0x79();
    void extendedOpcodeHandler0x7A();
    void extendedOpcodeHandler0x7B();
    void extendedOpcodeHandler0xA0();
    void extendedOpcodeHandler0xA1();
    void extendedOpcodeHandler0xA2();
    void extendedOpcodeHandler0xA3();
    void extendedOpcodeHandler0xA8();
    void extendedOpcodeHandler0xA9();
    void extendedOpcodeHandler0xAA();
    void extendedOpcodeHandler0xAB();
    void extendedOpcodeHandler0xB0();
    void extendedOpcodeHandler0xB1();
    void extendedOpcodeHandler0xB2();
    void extendedOpcodeHandler0xB3();
    void extendedOpcodeHandler0xB8();
    void extendedOpcodeHandler0xB9();
    void extendedOpcodeHandler0xBA();
    void extendedOpcodeHandler0xBB();

    void bitOpcodeHandler0x00();
    void bitOpcodeHandler0x01();
    void bitOpcodeHandler0x02();
    void bitOpcodeHandler0x03();
    void bitOpcodeHandler0x04();
    void bitOpcodeHandler0x05();
    void bitOpcodeHandler0x06();
    void bitOpcodeHandler0x07();
    void bitOpcodeHandler0x08();
    void bitOpcodeHandler0x09();
    void bitOpcodeHandler0x0A();
    void bitOpcodeHandler0x0B();
    void bitOpcodeHandler0x0C();
    void bitOpcodeHandler0x0D();
    void bitOpcodeHandler0x0E();
    void bitOpcodeHandler0x0F();
    void bitOpcodeHandler0x10();
    void bitOpcodeHandler0x11();
    void bitOpcodeHandler0x12();
    void bitOpcodeHandler0x13();
    void bitOpcodeHandler0x14();
    void bitOpcodeHandler0x15();
    void bitOpcodeHandler0x16();
    void bitOpcodeHandler0x17();
    void bitOpcodeHandler0x18();
    void bitOpcodeHandler0x19();
    void bitOpcodeHandler0x1A();
    void bitOpcodeHandler0x1B();
    void bitOpcodeHandler0x1C();
    void bitOpcodeHandler0x1D();
    void bitOpcodeHandler0x1E();
    void bitOpcodeHandler0x1F();
    void bitOpcodeHandler0x20();
    void bitOpcodeHandler0x21();
    void bitOpcodeHandler0x22();
    void bitOpcodeHandler0x23();
    void bitOpcodeHandler0x24();
    void bitOpcodeHandler0x25();
    void bitOpcodeHandler0x26();
    void bitOpcodeHandler0x27();
    void bitOpcodeHandler0x28();
    void bitOpcodeHandler0x29();
    void bitOpcodeHandler0x2A();
    void bitOpcodeHandler0x2B();
    void bitOpcodeHandler0x2C();
    void bitOpcodeHandler0x2D();
    void bitOpcodeHandler0x2E();
    void bitOpcodeHandler0x2F();
    void bitOpcodeHandler0x30();
    void bitOpcodeHandler0x31();
    void bitOpcodeHandler0x32();
    void bitOpcodeHandler0x33();
    void bitOpcodeHandler0x34();
    void bitOpcodeHandler0x35();
    void bitOpcodeHandler0x36();
    void bitOpcodeHandler0x37();
    void bitOpcodeHandler0x38();
    void bitOpcodeHandler0x39();
    void bitOpcodeHandler0x3A();
    void bitOpcodeHandler0x3B();
    void bitOpcodeHandler0x3C();
    void bitOpcodeHandler0x3D();
    void bitOpcodeHandler0x3E();
    void bitOpcodeHandler0x3F();
    void bitOpcodeHandler0x40();
    void bitOpcodeHandler0x41();
    void bitOpcodeHandler0x42();
    void bitOpcodeHandler0x43();
    void bitOpcodeHandler0x44();
    void bitOpcodeHandler0x45();
    void bitOpcodeHandler0x46();
    void bitOpcodeHandler0x47();
    void bitOpcodeHandler0x48();
    void bitOpcodeHandler0x49();
    void bitOpcodeHandler0x4A();
    void bitOpcodeHandler0x4B();
    void bitOpcodeHandler0x4C();
    void bitOpcodeHandler0x4D();
    void bitOpcodeHandler0x4E();
    void bitOpcodeHandler0x4F();
    void bitOpcodeHandler0x50();
    void bitOpcodeHandler0x51();
    void bitOpcodeHandler0x52();
    void bitOpcodeHandler0x53();
    void bitOpcodeHandler0x54();
    void bitOpcodeHandler0x55();
    void bitOpcodeHandler0x56();
    void bitOpcodeHandler0x57();
    void bitOpcodeHandler0x58();
    void bitOpcodeHandler0x59();
    void bitOpcodeHandler0x5A();
    void bitOpcodeHandler0x5B();
    void bitOpcodeHandler0x5C();
    void bitOpcodeHandler0x5D();
    void bitOpcodeHandler0x5E();
    void bitOpcodeHandler0x5F();
    void bitOpcodeHandler0x60();
    void bitOpcodeHandler0x61();
    void bitOpcodeHandler0x62();
    void bitOpcodeHandler0x63();
    void bitOpcodeHandler0x64();
    void bitOpcodeHandler0x65();
    void bitOpcodeHandler0x66();
    void bitOpcodeHandler0x67();
    void bitOpcodeHandler0x68();
    void bitOpcodeHandler0x69();
    void bitOpcodeHandler0x6A();
    void bitOpcodeHandler0x6B();
    void bitOpcodeHandler0x6C();
    void bitOpcodeHandler0x6D();
    void bitOpcodeHandler0x6E();
    void bitOpcodeHandler0x6F();
    void bitOpcodeHandler0x70();
    void bitOpcodeHandler0x71();
    void bitOpcodeHandler0x72();
    void bitOpcodeHandler0x73();
    void bitOpcodeHandler0x74();
    void bitOpcodeHandler0x75();
    void bitOpcodeHandler0x76();
    void bitOpcodeHandler0x77();
    void bitOpcodeHandler0x78();
    void bitOpcodeHandler0x79();
    void bitOpcodeHandler0x7A();
    void bitOpcodeHandler0x7B();
    void bitOpcodeHandler0x7C();
    void bitOpcodeHandler0x7D();
    void bitOpcodeHandler0x7E();
    void bitOpcodeHandler0x7F();
    void bitOpcodeHandler0x80();
    void bitOpcodeHandler0x81();
    void bitOpcodeHandler0x82();
    void bitOpcodeHandler0x83();
    void bitOpcodeHandler0x84();
    void bitOpcodeHandler0x85();
    void bitOpcodeHandler0x86();
    void bitOpcodeHandler0x87();
    void bitOpcodeHandler0x88();
    void bitOpcodeHandler0x89();
    void bitOpcodeHandler0x8A();
    void bitOpcodeHandler0x8B();
    void bitOpcodeHandler0x8C();
    void bitOpcodeHandler0x8D();
    void bitOpcodeHandler0x8E();
    void bitOpcodeHandler0x8F();
    void bitOpcodeHandler0x90();
    void bitOpcodeHandler0x91();
    void bitOpcodeHandler0x92();
    void bitOpcodeHandler0x93();
    void bitOpcodeHandler0x94();
    void bitOpcodeHandler0x95();
    void bitOpcodeHandler0x96();
    void bitOpcodeHandler0x97();
    void bitOpcodeHandler0x98();
    void bitOpcodeHandler0x99();
    void bitOpcodeHandler0x9A();
    void bitOpcodeHandler0x9B();
    void bitOpcodeHandler0x9C();
    void bitOpcodeHandler0x9D();
    void bitOpcodeHandler0x9E();
    void bitOpcodeHandler0x9F();
    void bitOpcodeHandler0xA0();
    void bitOpcodeHandler0xA1();
    void bitOpcodeHandler0xA2();
    void bitOpcodeHandler0xA3();
    void bitOpcodeHandler0xA4();
    void bitOpcodeHandler0xA5();
    void bitOpcodeHandler0xA6();
    void bitOpcodeHandler0xA7();
    void bitOpcodeHandler0xA8();
    void bitOpcodeHandler0xA9();
    void bitOpcodeHandler0xAA();
    void bitOpcodeHandler0xAB();
    void bitOpcodeHandler0xAC();
    void bitOpcodeHandler0xAD();
    void bitOpcodeHandler0xAE();
    void bitOpcodeHandler0xAF();
    void bitOpcodeHandler0xB0();
    void bitOpcodeHandler0xB1();
    void bitOpcodeHandler0xB2();
    void bitOpcodeHandler0xB3();
    void bitOpcodeHandler0xB4();
    void bitOpcodeHandler0xB5();
    void bitOpcodeHandler0xB6();
    void bitOpcodeHandler0xB7();
    void bitOpcodeHandler0xB8();
    void bitOpcodeHandler0xB9();
    void bitOpcodeHandler0xBA();
    void bitOpcodeHandler0xBB();
    void bitOpcodeHandler0xBC();
    void bitOpcodeHandler0xBD();
    void bitOpcodeHandler0xBE();
    void bitOpcodeHandler0xBF();
    void bitOpcodeHandler0xC0();
    void bitOpcodeHandler0xC1();
    void bitOpcodeHandler0xC2();
    void bitOpcodeHandler0xC3();
    void bitOpcodeHandler0xC4();
    void bitOpcodeHandler0xC5();
    void bitOpcodeHandler0xC6();
    void bitOpcodeHandler0xC7();
    void bitOpcodeHandler0xC8();
    void bitOpcodeHandler0xC9();
    void bitOpcodeHandler0xCA();
    void bitOpcodeHandler0xCB();
    void bitOpcodeHandler0xCC();
    void bitOpcodeHandler0xCD();
    void bitOpcodeHandler0xCE();
    void bitOpcodeHandler0xCF();
    void bitOpcodeHandler0xD0();
    void bitOpcodeHandler0xD1();
    void bitOpcodeHandler0xD2();
    void bitOpcodeHandler0xD3();
    void bitOpcodeHandler0xD4();
    void bitOpcodeHandler0xD5();
    void bitOpcodeHandler0xD6();
    void bitOpcodeHandler0xD7();
    void bitOpcodeHandler0xD8();
    void bitOpcodeHandler0xD9();
    void bitOpcodeHandler0xDA();
    void bitOpcodeHandler0xDB();
    void bitOpcodeHandler0xDC();
    void bitOpcodeHandler0xDD();
    void bitOpcodeHandler0xDE();
    void bitOpcodeHandler0xDF();
    void bitOpcodeHandler0xE0();
    void bitOpcodeHandler0xE1();
    void bitOpcodeHandler0xE2();
    void bitOpcodeHandler0xE3();
    void bitOpcodeHandler0xE4();
    void bitOpcodeHandler0xE5();
    void bitOpcodeHandler0xE6();
    void bitOpcodeHandler0xE7();
    void bitOpcodeHandler0xE8();
    void bitOpcodeHandler0xE9();
    void bitOpcodeHandler0xEA();
    void bitOpcodeHandler0xEB();
    void bitOpcodeHandler0xEC();
    void bitOpcodeHandler0xED();
    void bitOpcodeHandler0xEE();
    void bitOpcodeHandler0xEF();
    void bitOpcodeHandler0xF0();
    void bitOpcodeHandler0xF1();
    void bitOpcodeHandler0xF2();
    void bitOpcodeHandler0xF3();
    void bitOpcodeHandler0xF4();
    void bitOpcodeHandler0xF5();
    void bitOpcodeHandler0xF6();
    void bitOpcodeHandler0xF7();
    void bitOpcodeHandler0xF8();
    void bitOpcodeHandler0xF9();
    void bitOpcodeHandler0xFA();
    void bitOpcodeHandler0xFB();
    void bitOpcodeHandler0xFC();
    void bitOpcodeHandler0xFD();
    void bitOpcodeHandler0xFE();
    void bitOpcodeHandler0xFF();

    // TODO should indexRegister be a property which is set before calling these? refactor later if that is cleaner as this won't always be used.
    void invalidIndexOpcodeHandler();
    void indexOpcodeHandler0x04();
    void indexOpcodeHandler0x05();
    void indexOpcodeHandler0x06();
    void indexOpcodeHandler0x09();
    void indexOpcodeHandler0x0C();
    void indexOpcodeHandler0x0D();
    void indexOpcodeHandler0x0E();
    void indexOpcodeHandler0x14();
    void indexOpcodeHandler0x15();
    void indexOpcodeHandler0x16();
    void indexOpcodeHandler0x19();
    void indexOpcodeHandler0x1C();
    void indexOpcodeHandler0x1D();
    void indexOpcodeHandler0x1E();
    void indexOpcodeHandler0x21();
    void indexOpcodeHandler0x22();
    void indexOpcodeHandler0x23();
    void indexOpcodeHandler0x24();
    void indexOpcodeHandler0x25();
    void indexOpcodeHandler0x26();
    void indexOpcodeHandler0x29();
    void indexOpcodeHandler0x2A();
    void indexOpcodeHandler0x2B();
    void indexOpcodeHandler0x2C();
    void indexOpcodeHandler0x2D();
    void indexOpcodeHandler0x2E();
    void indexOpcodeHandler0x34();
    void indexOpcodeHandler0x35();
    void indexOpcodeHandler0x36();
    void indexOpcodeHandler0x39();
    void indexOpcodeHandler0x3C();
    void indexOpcodeHandler0x3D();
    void indexOpcodeHandler0x3E();
    void indexOpcodeHandler0x40();
    void indexOpcodeHandler0x41();
    void indexOpcodeHandler0x42();
    void indexOpcodeHandler0x43();
    void indexOpcodeHandler0x44();
    void indexOpcodeHandler0x45();
    void indexOpcodeHandler0x46();
    void indexOpcodeHandler0x47();
    void indexOpcodeHandler0x48();
    void indexOpcodeHandler0x49();
    void indexOpcodeHandler0x4A();
    void indexOpcodeHandler0x4B();
    void indexOpcodeHandler0x4C();
    void indexOpcodeHandler0x4D();
    void indexOpcodeHandler0x4E();
    void indexOpcodeHandler0x4F();
    void indexOpcodeHandler0x50();
    void indexOpcodeHandler0x51();
    void indexOpcodeHandler0x52();
    void indexOpcodeHandler0x53();
    void indexOpcodeHandler0x54();
    void indexOpcodeHandler0x55();
    void indexOpcodeHandler0x56();
    void indexOpcodeHandler0x57();
    void indexOpcodeHandler0x58();
    void indexOpcodeHandler0x59();
    void indexOpcodeHandler0x5A();
    void indexOpcodeHandler0x5B();
    void indexOpcodeHandler0x5C();
    void indexOpcodeHandler0x5D();
    void indexOpcodeHandler0x5E();
    void indexOpcodeHandler0x5F();
    void indexOpcodeHandler0x60();
    void indexOpcodeHandler0x61();
    void indexOpcodeHandler0x62();
    void indexOpcodeHandler0x63();
    void indexOpcodeHandler0x64();
    void indexOpcodeHandler0x65();
    void indexOpcodeHandler0x66();
    void indexOpcodeHandler0x67();
    void indexOpcodeHandler0x68();
    void indexOpcodeHandler0x69();
    void indexOpcodeHandler0x6A();
    void indexOpcodeHandler0x6B();
    void indexOpcodeHandler0x6C();
    void indexOpcodeHandler0x6D();
    void indexOpcodeHandler0x6E();
    void indexOpcodeHandler0x6F();
    void indexOpcodeHandler0x70();
    void indexOpcodeHandler0x71();
    void indexOpcodeHandler0x72();
    void indexOpcodeHandler0x73();
    void indexOpcodeHandler0x74();
    void indexOpcodeHandler0x75();
    void indexOpcodeHandler0x77();
    void indexOpcodeHandler0x78();
    void indexOpcodeHandler0x79();
    void indexOpcodeHandler0x7A();
    void indexOpcodeHandler0x7B();
    void indexOpcodeHandler0x7C();
    void indexOpcodeHandler0x7D();
    void indexOpcodeHandler0x7E();
    void indexOpcodeHandler0x7F();
    void indexOpcodeHandler0x80();
    void indexOpcodeHandler0x81();
    void indexOpcodeHandler0x82();
    void indexOpcodeHandler0x83();
    void indexOpcodeHandler0x84();
    void indexOpcodeHandler0x85();
    void indexOpcodeHandler0x86();
    void indexOpcodeHandler0x87();
    void indexOpcodeHandler0x88();
    void indexOpcodeHandler0x89();
    void indexOpcodeHandler0x8A();
    void indexOpcodeHandler0x8B();
    void indexOpcodeHandler0x8C();
    void indexOpcodeHandler0x8D();
    void indexOpcodeHandler0x8E();
    void indexOpcodeHandler0x8F();
    void indexOpcodeHandler0x90();
    void indexOpcodeHandler0x91();
    void indexOpcodeHandler0x92();
    void indexOpcodeHandler0x93();
    void indexOpcodeHandler0x94();
    void indexOpcodeHandler0x95();
    void indexOpcodeHandler0x96();
    void indexOpcodeHandler0x97();
    void indexOpcodeHandler0x98();
    void indexOpcodeHandler0x99();
    void indexOpcodeHandler0x9A();
    void indexOpcodeHandler0x9B();
    void indexOpcodeHandler0x9C();
    void indexOpcodeHandler0x9D();
    void indexOpcodeHandler0x9E();
    void indexOpcodeHandler0x9F();
    void indexOpcodeHandler0xA0();
    void indexOpcodeHandler0xA1();
    void indexOpcodeHandler0xA2();
    void indexOpcodeHandler0xA3();
    void indexOpcodeHandler0xA4();
    void indexOpcodeHandler0xA5();
    void indexOpcodeHandler0xA6();
    void indexOpcodeHandler0xA7();
    void indexOpcodeHandler0xA8();
    void indexOpcodeHandler0xA9();
    void indexOpcodeHandler0xAA();
    void indexOpcodeHandler0xAB();
    void indexOpcodeHandler0xAC();
    void indexOpcodeHandler0xAD();
    void indexOpcodeHandler0xAE();
    void indexOpcodeHandler0xAF();
    void indexOpcodeHandler0xB0();
    void indexOpcodeHandler0xB1();
    void indexOpcodeHandler0xB2();
    void indexOpcodeHandler0xB3();
    void indexOpcodeHandler0xB4();
    void indexOpcodeHandler0xB5();
    void indexOpcodeHandler0xB6();
    void indexOpcodeHandler0xB7();
    void indexOpcodeHandler0xB8();
    void indexOpcodeHandler0xB9();
    void indexOpcodeHandler0xBA();
    void indexOpcodeHandler0xBB();
    void indexOpcodeHandler0xBC();
    void indexOpcodeHandler0xBD();
    void indexOpcodeHandler0xBE();
    void indexOpcodeHandler0xBF();
    void indexOpcodeHandler0xCB();
    void indexOpcodeHandler0xE1();
    void indexOpcodeHandler0xE3();
    void indexOpcodeHandler0xE5();
    void indexOpcodeHandler0xE9();
    void indexOpcodeHandler0xF9();

    void indexBitOpcodeHandler0x00();
    void indexBitOpcodeHandler0x01();
    void indexBitOpcodeHandler0x02();
    void indexBitOpcodeHandler0x03();
    void indexBitOpcodeHandler0x04();
    void indexBitOpcodeHandler0x05();
    void indexBitOpcodeHandler0x06();
    void indexBitOpcodeHandler0x07();
    void indexBitOpcodeHandler0x08();
    void indexBitOpcodeHandler0x09();
    void indexBitOpcodeHandler0x0A();
    void indexBitOpcodeHandler0x0B();
    void indexBitOpcodeHandler0x0C();
    void indexBitOpcodeHandler0x0D();
    void indexBitOpcodeHandler0x0E();
    void indexBitOpcodeHandler0x0F();
    void indexBitOpcodeHandler0x10();
    void indexBitOpcodeHandler0x11();
    void indexBitOpcodeHandler0x12();
    void indexBitOpcodeHandler0x13();
    void indexBitOpcodeHandler0x14();
    void indexBitOpcodeHandler0x15();
    void indexBitOpcodeHandler0x16();
    void indexBitOpcodeHandler0x17();
    void indexBitOpcodeHandler0x18();
    void indexBitOpcodeHandler0x19();
    void indexBitOpcodeHandler0x1A();
    void indexBitOpcodeHandler0x1B();
    void indexBitOpcodeHandler0x1C();
    void indexBitOpcodeHandler0x1D();
    void indexBitOpcodeHandler0x1E();
    void indexBitOpcodeHandler0x1F();
    void indexBitOpcodeHandler0x20();
    void indexBitOpcodeHandler0x21();
    void indexBitOpcodeHandler0x22();
    void indexBitOpcodeHandler0x23();
    void indexBitOpcodeHandler0x24();
    void indexBitOpcodeHandler0x25();
    void indexBitOpcodeHandler0x26();
    void indexBitOpcodeHandler0x27();
    void indexBitOpcodeHandler0x28();
    void indexBitOpcodeHandler0x29();
    void indexBitOpcodeHandler0x2A();
    void indexBitOpcodeHandler0x2B();
    void indexBitOpcodeHandler0x2C();
    void indexBitOpcodeHandler0x2D();
    void indexBitOpcodeHandler0x2E();
    void indexBitOpcodeHandler0x2F();
    void indexBitOpcodeHandler0x30();
    void indexBitOpcodeHandler0x31();
    void indexBitOpcodeHandler0x32();
    void indexBitOpcodeHandler0x33();
    void indexBitOpcodeHandler0x34();
    void indexBitOpcodeHandler0x35();
    void indexBitOpcodeHandler0x36();
    void indexBitOpcodeHandler0x37();
    void indexBitOpcodeHandler0x38();
    void indexBitOpcodeHandler0x39();
    void indexBitOpcodeHandler0x3A();
    void indexBitOpcodeHandler0x3B();
    void indexBitOpcodeHandler0x3C();
    void indexBitOpcodeHandler0x3D();
    void indexBitOpcodeHandler0x3E();
    void indexBitOpcodeHandler0x3F();
    void indexBitOpcodeHandler0x40();
    void indexBitOpcodeHandler0x41();
    void indexBitOpcodeHandler0x42();
    void indexBitOpcodeHandler0x43();
    void indexBitOpcodeHandler0x44();
    void indexBitOpcodeHandler0x45();
    void indexBitOpcodeHandler0x46();
    void indexBitOpcodeHandler0x47();
    void indexBitOpcodeHandler0x48();
    void indexBitOpcodeHandler0x49();
    void indexBitOpcodeHandler0x4A();
    void indexBitOpcodeHandler0x4B();
    void indexBitOpcodeHandler0x4C();
    void indexBitOpcodeHandler0x4D();
    void indexBitOpcodeHandler0x4E();
    void indexBitOpcodeHandler0x4F();
    void indexBitOpcodeHandler0x50();
    void indexBitOpcodeHandler0x51();
    void indexBitOpcodeHandler0x52();
    void indexBitOpcodeHandler0x53();
    void indexBitOpcodeHandler0x54();
    void indexBitOpcodeHandler0x55();
    void indexBitOpcodeHandler0x56();
    void indexBitOpcodeHandler0x57();
    void indexBitOpcodeHandler0x58();
    void indexBitOpcodeHandler0x59();
    void indexBitOpcodeHandler0x5A();
    void indexBitOpcodeHandler0x5B();
    void indexBitOpcodeHandler0x5C();
    void indexBitOpcodeHandler0x5D();
    void indexBitOpcodeHandler0x5E();
    void indexBitOpcodeHandler0x5F();
    void indexBitOpcodeHandler0x60();
    void indexBitOpcodeHandler0x61();
    void indexBitOpcodeHandler0x62();
    void indexBitOpcodeHandler0x63();
    void indexBitOpcodeHandler0x64();
    void indexBitOpcodeHandler0x65();
    void indexBitOpcodeHandler0x66();
    void indexBitOpcodeHandler0x67();
    void indexBitOpcodeHandler0x68();
    void indexBitOpcodeHandler0x69();
    void indexBitOpcodeHandler0x6A();
    void indexBitOpcodeHandler0x6B();
    void indexBitOpcodeHandler0x6C();
    void indexBitOpcodeHandler0x6D();
    void indexBitOpcodeHandler0x6E();
    void indexBitOpcodeHandler0x6F();
    void indexBitOpcodeHandler0x70();
    void indexBitOpcodeHandler0x71();
    void indexBitOpcodeHandler0x72();
    void indexBitOpcodeHandler0x73();
    void indexBitOpcodeHandler0x74();
    void indexBitOpcodeHandler0x75();
    void indexBitOpcodeHandler0x76();
    void indexBitOpcodeHandler0x77();
    void indexBitOpcodeHandler0x78();
    void indexBitOpcodeHandler0x79();
    void indexBitOpcodeHandler0x7A();
    void indexBitOpcodeHandler0x7B();
    void indexBitOpcodeHandler0x7C();
    void indexBitOpcodeHandler0x7D();
    void indexBitOpcodeHandler0x7E();
    void indexBitOpcodeHandler0x7F();
    void indexBitOpcodeHandler0x80();
    void indexBitOpcodeHandler0x81();
    void indexBitOpcodeHandler0x82();
    void indexBitOpcodeHandler0x83();
    void indexBitOpcodeHandler0x84();
    void indexBitOpcodeHandler0x85();
    void indexBitOpcodeHandler0x86();
    void indexBitOpcodeHandler0x87();
    void indexBitOpcodeHandler0x88();
    void indexBitOpcodeHandler0x89();
    void indexBitOpcodeHandler0x8A();
    void indexBitOpcodeHandler0x8B();
    void indexBitOpcodeHandler0x8C();
    void indexBitOpcodeHandler0x8D();
    void indexBitOpcodeHandler0x8E();
    void indexBitOpcodeHandler0x8F();
    void indexBitOpcodeHandler0x90();
    void indexBitOpcodeHandler0x91();
    void indexBitOpcodeHandler0x92();
    void indexBitOpcodeHandler0x93();
    void indexBitOpcodeHandler0x94();
    void indexBitOpcodeHandler0x95();
    void indexBitOpcodeHandler0x96();
    void indexBitOpcodeHandler0x97();
    void indexBitOpcodeHandler0x98();
    void indexBitOpcodeHandler0x99();
    void indexBitOpcodeHandler0x9A();
    void indexBitOpcodeHandler0x9B();
    void indexBitOpcodeHandler0x9C();
    void indexBitOpcodeHandler0x9D();
    void indexBitOpcodeHandler0x9E();
    void indexBitOpcodeHandler0x9F();
    void indexBitOpcodeHandler0xA0();
    void indexBitOpcodeHandler0xA1();
    void indexBitOpcodeHandler0xA2();
    void indexBitOpcodeHandler0xA3();
    void indexBitOpcodeHandler0xA4();
    void indexBitOpcodeHandler0xA5();
    void indexBitOpcodeHandler0xA6();
    void indexBitOpcodeHandler0xA7();
    void indexBitOpcodeHandler0xA8();
    void indexBitOpcodeHandler0xA9();
    void indexBitOpcodeHandler0xAA();
    void indexBitOpcodeHandler0xAB();
    void indexBitOpcodeHandler0xAC();
    void indexBitOpcodeHandler0xAD();
    void indexBitOpcodeHandler0xAE();
    void indexBitOpcodeHandler0xAF();
    void indexBitOpcodeHandler0xB0();
    void indexBitOpcodeHandler0xB1();
    void indexBitOpcodeHandler0xB2();
    void indexBitOpcodeHandler0xB3();
    void indexBitOpcodeHandler0xB4();
    void indexBitOpcodeHandler0xB5();
    void indexBitOpcodeHandler0xB6();
    void indexBitOpcodeHandler0xB7();
    void indexBitOpcodeHandler0xB8();
    void indexBitOpcodeHandler0xB9();
    void indexBitOpcodeHandler0xBA();
    void indexBitOpcodeHandler0xBB();
    void indexBitOpcodeHandler0xBC();
    void indexBitOpcodeHandler0xBD();
    void indexBitOpcodeHandler0xBE();
    void indexBitOpcodeHandler0xBF();
    void indexBitOpcodeHandler0xC0();
    void indexBitOpcodeHandler0xC1();
    void indexBitOpcodeHandler0xC2();
    void indexBitOpcodeHandler0xC3();
    void indexBitOpcodeHandler0xC4();
    void indexBitOpcodeHandler0xC5();
    void indexBitOpcodeHandler0xC6();
    void indexBitOpcodeHandler0xC7();
    void indexBitOpcodeHandler0xC8();
    void indexBitOpcodeHandler0xC9();
    void indexBitOpcodeHandler0xCA();
    void indexBitOpcodeHandler0xCB();
    void indexBitOpcodeHandler0xCC();
    void indexBitOpcodeHandler0xCD();
    void indexBitOpcodeHandler0xCE();
    void indexBitOpcodeHandler0xCF();
    void indexBitOpcodeHandler0xD0();
    void indexBitOpcodeHandler0xD1();
    void indexBitOpcodeHandler0xD2();
    void indexBitOpcodeHandler0xD3();
    void indexBitOpcodeHandler0xD4();
    void indexBitOpcodeHandler0xD5();
    void indexBitOpcodeHandler0xD6();
    void indexBitOpcodeHandler0xD7();
    void indexBitOpcodeHandler0xD8();
    void indexBitOpcodeHandler0xD9();
    void indexBitOpcodeHandler0xDA();
    void indexBitOpcodeHandler0xDB();
    void indexBitOpcodeHandler0xDC();
    void indexBitOpcodeHandler0xDD();
    void indexBitOpcodeHandler0xDE();
    void indexBitOpcodeHandler0xDF();
    void indexBitOpcodeHandler0xE0();
    void indexBitOpcodeHandler0xE1();
    void indexBitOpcodeHandler0xE2();
    void indexBitOpcodeHandler0xE3();
    void indexBitOpcodeHandler0xE4();
    void indexBitOpcodeHandler0xE5();
    void indexBitOpcodeHandler0xE6();
    void indexBitOpcodeHandler0xE7();
    void indexBitOpcodeHandler0xE8();
    void indexBitOpcodeHandler0xE9();
    void indexBitOpcodeHandler0xEA();
    void indexBitOpcodeHandler0xEB();
    void indexBitOpcodeHandler0xEC();
    void indexBitOpcodeHandler0xED();
    void indexBitOpcodeHandler0xEE();
    void indexBitOpcodeHandler0xEF();
    void indexBitOpcodeHandler0xF0();
    void indexBitOpcodeHandler0xF1();
    void indexBitOpcodeHandler0xF2();
    void indexBitOpcodeHandler0xF3();
    void indexBitOpcodeHandler0xF4();
    void indexBitOpcodeHandler0xF5();
    void indexBitOpcodeHandler0xF6();
    void indexBitOpcodeHandler0xF7();
    void indexBitOpcodeHandler0xF8();
    void indexBitOpcodeHandler0xF9();
    void indexBitOpcodeHandler0xFA();
    void indexBitOpcodeHandler0xFB();
    void indexBitOpcodeHandler0xFC();
    void indexBitOpcodeHandler0xFD();
    void indexBitOpcodeHandler0xFE();
    void indexBitOpcodeHandler0xFF();
};
