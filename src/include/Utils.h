#ifndef UTILS_INCLUDED
#define UTILS_INCLUDED

#include <vector>
#include <iomanip>
#include <ctime>

enum class DateFormat {
    FORMAT_ISO, // Y-m-d
    FORMAT_DATETIME_ISO, // Y-m-d H:M:S
    FORMAT_UK, // d/m/Y
    FORMAT_DATETIME_UK, // d/m/Y H:M:S
    FORMAT_US, // m/d/Y
    FORMAT_DATETIME_US // m/d/Y H:M:S
};

class Utils {
public:
    static void setBit(int bit, bool val, unsigned char &variable);

    static void setBit(int bit, bool val, unsigned short &variable);

    static bool testBit(int bit, const unsigned char variable);

    static bool testBit(int bit, const unsigned short variable);

    static std::string getVersionString(bool verboseString);

    static bool fileExists(const std::string &fileName);

    static std::vector<std::string> explodeString(const std::string &s, char delimiter);

    static std::string implodeString(std::vector<std::string> explodedString, std::string glue, unsigned int count);

    static std::string implodeString(std::vector<std::string> explodedString, std::string glue);

    static std::string implodeString(std::vector<std::string> explodedString);

    static std::string removeQuotationsFromString(std::string string);

    static std::string formatDate(const std::string &date, DateFormat dateFormat);

    static std::string getSystemDateTime(DateFormat dateFormat);

    static std::vector<int> getMonthLengthMatrix(bool leapYear);

    static bool isAcceptedValue(std::vector<std::string> acceptedValues, std::string subject, bool caseInsensitive);

    static std::string strToLower(std::string subject);

    static std::string padString(const std::string &string, unsigned long length);

    static std::string formatHexNumber(unsigned short value);

    static std::string formatHexNumber(unsigned char value);

private:
};

#endif