class Utils {
public:
  static void setBit(int bit, bool val, unsigned char &variable);
  static void setBit(int bit, bool val, unsigned short &variable);
  static bool testBit(int bit, unsigned char variable);
  static bool testBit(int bit, unsigned short variable);
private:
};
