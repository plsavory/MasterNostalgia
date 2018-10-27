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

// TODO: Ensure that the bits are actually in the correct orientation with testing
enum cpuFlag: unsigned char {
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
  cpuRegister gpRegisters[10]; // General purpose CPU registers TODO: Handle shadow register updating each cycle
  cpuState state;
  void extendedOpcodes(unsigned char opcode);
  void executeOpcode();
  Memory *memory;
  int cyclesTaken;
  unsigned char NB();
  int interruptMode; // TODO: Use http://z80.info/1653.htm as reference when implementing interrupts in future.
  void logCPUState(unsigned char opcode, std::string prefix);
  // Instruction handler functions
  void ldReg8(unsigned char &dest, unsigned char value);
  void ldReg16(cpuRegister &dest, unsigned short value);
  void adc(unsigned char &dest, unsigned char value);
  void sbc(unsigned char &dest, unsigned char value);
  void setInterruptMode(int mode);
};
