class Utils {
public:
  static void setBit(int bit, bool val, unsigned char &variable);
  static void setBit(int bit, bool val, unsigned short &variable);
  static bool testBit(int bit, const unsigned char variable);
  static bool testBit(int bit, const unsigned short variable);
  static std::string getVersionString(bool verboseString);
private:
};
