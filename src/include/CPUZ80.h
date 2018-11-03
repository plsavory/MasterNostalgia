/*
Mastalgia - a (soon to be) Sega Master System emulator.
Licensed under the GPLv3 license.
@author: Peter Savory
 */

enum cpuState {
  Halt, Running, Error, Step
};

// For a nice way to address the CPU registers in the code...
enum cpuReg {
  AF,BC,DE,HL,AFS,BCS,DES,HLS,rIX,rIY
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
  NZ,Z,NC,C,PO,PE,P,M
};

// TODO: Ensure that the bits are actually in the correct orientation with testing
enum CPUFlag: unsigned char {
  carry = 1 << 0,
  subtract = 1 << 1,
  overflow = 1 << 2,
  unused2 = 1 << 3,
  halfCarry = 1 << 4,
  unused = 1 << 5,
  zero = 1 << 6,
  sign = 1 << 7
};

// Force these variables to use the same memory space - a handy way of emulating the CPU registers.
union cpuRegister {
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
private:
  unsigned short programCounter;
  unsigned short stackPointer;
  unsigned char flags;
  cpuRegister gpRegisters[10]; // General purpose CPU registers TODO: Handle shadow register updating each cycle
  cpuState state;
  void extendedOpcodes(unsigned char opcode);
  void executeOpcode();
  Memory *memory;
  int cyclesTaken;
  unsigned char NB();
  int interruptMode; // TODO: Use http://z80.info/1653.htm as reference when implementing interrupts in future.
  void logCPUState(unsigned char opcode, std::string prefix);
  void jpCondition(JPCondition condition, unsigned char location);
  // Instruction handler functions
  void ldReg8(unsigned char &dest, unsigned char value);
  void ldReg16(cpuRegister &dest, unsigned short value);
  void adc(unsigned char &dest, unsigned char value);
  void sbc(unsigned char &dest, unsigned char value);
  void setInterruptMode(int mode);
  // To make flag handling easier...
  void setFlag(CPUFlag flag, bool value);
  bool getFlag(CPUFlag flag);
  // Memory management
  unsigned short build16BitAddress();
};
